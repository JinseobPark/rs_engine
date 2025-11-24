# Multi-View Geometry 기본 요소

Surround View에서는 여러 대의 카메라가 같은 씬을 다른 위치에서 촬영하므로,
카메라들 사이의 기하 관계를 이해하는 것이 중요하다.

---

## 1. Epipolar Geometry (에피폴라 기하)
두 대의 카메라가 있을 때, 한 카메라의 한 점이 다른 카메라에서 어디에 나타날 수 있는지 제약을 주는 기하 구조.

- **Epipole (에피폴)**: 한 카메라의 중심이 다른 카메라 이미지 평면 위에 투영된 점
- **Epipolar line (에피폴라 직선)**: 한 카메라의 한 3D 점에 대해, 다른 카메라 이미지 상에서 가능한 점들의 궤적 (직선)
- **Epipolar plane**: 두 카메라의 중심과 3D 점이 만드는 평면

이 구조 덕분에, 한 카메라에서의 점 대응을 찾을 때 2D 전체가 아니라 1D 직선 위에서만 탐색하면 되므로,
스테레오 매칭 등에서 계산량과 모호성이 줄어든다.

---

## 2. Essential Matrix (E)
두 카메라 사이의 **상대적인 회전과 평행이동**만을 이용해 정의되는 \(3\times3\) 행렬.

- 카메라가 정규화 좌표(카메라 좌표 → z로 나눈 좌표)로 표현되어 있을 때 사용
- Intrinsic이 제거된 상태, 즉 \(K\)를 고려하지 않은 좌표계

정규화 좌표계에서 점 \(x, x'\) (두 카메라 이미지 상의 대응점)에 대해:

$$
 x'^T E x = 0
$$

여기서

$$
E = [t]_\times R
$$

- \(R\): 첫 번째 카메라에서 두 번째 카메라로의 회전
- \(t\): 첫 번째 카메라에서 두 번째 카메라로의 translation 벡터
- \([t]_\times\): 벡터 \(t\)에 대한 skew-symmetric 행렬 (cross product 연산 표현)

---

## 3. Fundamental Matrix (F)
픽셀 좌표 (intrinsic을 포함한 좌표계)에서 두 이미지 사이의 제약을 주는 \(3\times3\) 행렬.

- 카메라 Intrinsic을 포함한 일반적인 이미지 좌표에서 사용
- 두 이미지 상의 대응점 \(x, x'\) (픽셀 동차 좌표)에 대해:

$$
 x'^T F x = 0
$$

Essential matrix와의 관계:

$$
F = K'^{-1} E K^{-1}
$$

- \(K, K'\): 두 카메라의 intrinsic 행렬
- \(E\): 두 카메라의 essential matrix

즉, \(F\)는 실제 픽셀 좌표계에서 에피폴라 기하를 표현하는 행렬이다.

---

## 4. Homography vs Fundamental Matrix
- **Homography (H)**
  - 한 평면 위에 있는 점들에 대해서만 유효한 2D ↔ 2D 사상
  - 예: Ground plane 위의 점들만 고려한 이미지 ↔ top-view 변환
- **Fundamental Matrix (F)**
  - 평면에 제한되지 않은 일반적인 3D 씬에 대해, 두 뷰 사이의 에피폴라 제약 표현

요약하면:
- 씬이 **한 평면에 가깝다** → Homography로 충분
- 씬이 **일반적인 3D 구조** → Fundamental Matrix / Essential Matrix 기반 Multi-View Geometry 필요

---

## 5. Surround View와 Multi-View Geometry
Surround View에서는 보통 각 카메라가 모두 차량 주변의 **바닥 면(ground plane)**을 중심으로 보기 때문에,
실무에서는 homography(ground-plane mapping)가 가장 핵심이 된다.

다만, 카메라 간의 상대 위치를 추정하거나, 단순 ground plane을 넘어 3D 구조(차량, 장애물 높이 등)를 재구성하고 싶을 때는
Multi-View Geometry의 다음 요소들이 중요해진다.

- 카메라 간 Extrinsic (R, t) 추정
- Essential / Fundamental Matrix를 이용한 대응점 제약
- Epipolar geometry 기반의 3D 재구성 (triangulation)

포트폴리오에서는 "Surround View는 기본적으로 ground-plane homography에 기반하지만,
멀티 카메라 기하(E, F, epipolar geometry)를 이해하고 있어야 확장된 3D 인지·재구성으로 확장 가능"하다는 식으로 정리하면 좋다.
