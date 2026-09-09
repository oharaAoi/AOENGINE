#include "LinePathMover.h"

/// stl
#include <algorithm>
#include <cmath>

namespace{
	/// 向きが求められないとみなす長さの下限
	constexpr float kDirectionEpsilon = 1.0e-6f;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ pointsの設定
///////////////////////////////////////////////////////////////////////////////////////////////

void LinePathMover::SetPoints(const std::vector<Math::Vector3>& points, bool isClosed){
	points_ = points;
	// 閉じたLineは終点から始点へ戻るセグメントが増えるため、3点以上必要になる
	isClosed_ = isClosed && points_.size() >= 3;

	BuildLengths();
	Reset();
}

void LinePathMover::SetLineData(const LineLoader::LineData& lineData){
	SetPoints(lineData.points, lineData.isClosed);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 全セグメントの距離から総距離を求める
///////////////////////////////////////////////////////////////////////////////////////////////

void LinePathMover::BuildLengths(){
	accumulatedLengths_.clear();
	totalLength_ = 0.0f;

	if(points_.size() < 2){
		return;
	}

	const size_t segmentCount = GetSegmentCount();
	accumulatedLengths_.reserve(segmentCount + 1);
	// 始点までの距離は0
	accumulatedLengths_.push_back(0.0f);

	for(size_t i = 0; i < segmentCount; ++i){
		const Math::Vector3& begin = points_[i];
		const Math::Vector3& end = points_[(i + 1) % points_.size()];

		totalLength_ += (end - begin).Length();
		accumulatedLengths_.push_back(totalLength_);
	}
}

size_t LinePathMover::GetSegmentCount() const{
	if(points_.size() < 2){
		return 0;
	}

	return isClosed_ ? points_.size() : points_.size() - 1;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void LinePathMover::Reset(){
	distance_ = 0.0f;
	velocity_ = params_.initVelocity;
	moveSign_ = 1.0f;
	isFinished_ = false;

	// 動き出す前から進行方向を向かせておく
	UpdateTransform(0.0f, true);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void LinePathMover::Update(float deltaTime){
	if(!IsValid() || isFinished_){
		return;
	}

	// -------------------------------------------------
	// ↓ 加速度で速さを更新する
	// -------------------------------------------------

	const float maxVelocity = std::max(0.0f, params_.maxVelocity);
	velocity_ = std::clamp(velocity_ + params_.accel * deltaTime, 0.0f, maxVelocity);

	// -------------------------------------------------
	// ↓ 1次元の進捗を進める
	// -------------------------------------------------

	distance_ += velocity_ * moveSign_ * deltaTime;
	ResolveDistance();

	UpdateTransform(deltaTime, false);
}

void LinePathMover::ResolveDistance(){
	if(totalLength_ <= 0.0f){
		distance_ = 0.0f;
		return;
	}

	switch(endAction_){
	case EndAction::Stop:
		// 終端で止める
		if(distance_ >= totalLength_){
			distance_ = totalLength_;
			isFinished_ = true;
		} else if(distance_ <= 0.0f){
			distance_ = 0.0f;
			// 逆向きに進んでいて始点まで戻った場合も終了とする
			if(moveSign_ < 0.0f){
				isFinished_ = true;
			}
		}
		break;

	case EndAction::Loop:
		// 総距離で折り返して始点へ戻す
		distance_ = std::fmod(distance_, totalLength_);
		if(distance_ < 0.0f){
			distance_ += totalLength_;
		}
		break;

	case EndAction::PingPong:
		// 終端を越えた分だけ内側へ跳ね返し、進む向きを反転させる
		while(distance_ < 0.0f || distance_ > totalLength_){
			if(distance_ > totalLength_){
				distance_ = (totalLength_ * 2.0f) - distance_;
			} else{
				distance_ = -distance_;
			}
			moveSign_ = -moveSign_;
		}
		break;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 座標と進行方向を向く回転の計算
///////////////////////////////////////////////////////////////////////////////////////////////

void LinePathMover::UpdateTransform(float deltaTime, bool isSnapRotate){
	if(points_.empty()){
		return;
	}

	position_ = GetPointAtDistance(distance_);

	// 進む向きが逆なら進行方向も逆になる
	const Math::Vector3 forward = GetDirectionAtDistance(distance_) * moveSign_;
	if(forward.Length() <= kDirectionEpsilon){
		// 向きが求められない場合は直前の回転を保つ
		return;
	}

	direction_ = forward.Normalize();

	const Math::Quaternion target = Math::Quaternion::LookRotation(direction_);
	if(isSnapRotate || params_.rotateLerpRate <= 0.0f){
		rotate_ = target;
		return;
	}

	// 進行方向へ徐々に向かせる
	const float rate = std::clamp(params_.rotateLerpRate * deltaTime, 0.0f, 1.0f);
	rotate_ = Math::Quaternion::Slerp(rotate_, target, rate);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 指定した距離の座標 / 進行方向
///////////////////////////////////////////////////////////////////////////////////////////////

size_t LinePathMover::FindSegment(float distance, float& rate) const{
	rate = 0.0f;

	const size_t segmentCount = GetSegmentCount();
	if(segmentCount == 0 || accumulatedLengths_.size() < 2){
		return 0;
	}

	const float clamped = std::clamp(distance, 0.0f, totalLength_);

	// 累積距離は昇順に並んでいるため二分探索で該当するセグメントを探す
	auto it = std::upper_bound(accumulatedLengths_.begin(), accumulatedLengths_.end(), clamped);
	size_t index = static_cast<size_t>(std::distance(accumulatedLengths_.begin(), it));
	index = std::clamp<size_t>(index, 1, segmentCount) - 1;

	const float segmentLength = accumulatedLengths_[index + 1] - accumulatedLengths_[index];
	if(segmentLength > 0.0f){
		rate = (clamped - accumulatedLengths_[index]) / segmentLength;
	}

	return index;
}

Math::Vector3 LinePathMover::GetPointAtDistance(float distance) const{
	if(points_.empty()){
		return CVector3::ZERO;
	}

	if(!IsValid()){
		return points_.front();
	}

	float rate = 0.0f;
	const size_t index = FindSegment(distance, rate);

	return Math::Vector3::Lerp(points_[index], points_[(index + 1) % points_.size()], rate);
}

Math::Vector3 LinePathMover::GetDirectionAtDistance(float distance) const{
	if(!IsValid()){
		return CVector3::ZERO;
	}

	float rate = 0.0f;
	const size_t index = FindSegment(distance, rate);

	const Math::Vector3 segment = points_[(index + 1) % points_.size()] - points_[index];
	if(segment.Length() <= kDirectionEpsilon){
		return CVector3::ZERO;
	}

	return segment.Normalize();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ setter
///////////////////////////////////////////////////////////////////////////////////////////////

void LinePathMover::SetDistance(float distance){
	distance_ = distance;
	isFinished_ = false;

	ResolveDistance();
	UpdateTransform(0.0f, true);
}

void LinePathMover::SetVelocity(float velocity){
	velocity_ = std::max(0.0f, velocity);
}

void LinePathMover::SetReverse(bool isReverse){
	moveSign_ = isReverse ? -1.0f : 1.0f;
	isFinished_ = false;

	UpdateTransform(0.0f, true);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ getter
///////////////////////////////////////////////////////////////////////////////////////////////

float LinePathMover::GetProgress() const{
	if(totalLength_ <= 0.0f){
		return 0.0f;
	}

	return distance_ / totalLength_;
}

bool LinePathMover::IsValid() const{
	return points_.size() >= 2 && totalLength_ > 0.0f;
}
