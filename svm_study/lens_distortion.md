# Lens Distortion 모델 정리

## 1. 왜곡(Distortion)이란?
실제 세계의 직선이 이미지에서 휘어 보이는 현상.
주로 렌즈 설계와 촬영 각도 때문에 발생하며, 카메라 보정(calibration) 과정에서 모델링 후 보정(undistort)한다.

대표적인 성분:
- Radial distortion (방사 왜곡)
- Tangential distortion (접선 왜곡 / 비대칭 정렬)

---

## 2. Brown–Conrady Distortion 모델
전통적인 pinhole 카메라 모델 주변에서 많이 사용하는 왜곡 모델.
OpenCV의 기본 `cv::calibrateCamera` 모델이 이 계열.

### 2.1. Radial Distortion
정규화 좌표계에서의 점을 \((x, y)\), 반지름을 \(r^2 = x^2 + y^2\)라고 하면:

$$
\begin{aligned}
 x_\text{radial} &= x \cdot (1 + k_1 r^2 + k_2 r^4 + k_3 r^6 + \dots) \\
 y_\text{radial} &= y \cdot (1 + k_1 r^2 + k_2 r^4 + k_3 r^6 + \dots)
\end{aligned}
$$

- \(k_1, k_2, k_3, \dots\): radial distortion 계수
- 배럴 왜곡(barrel): 보통 \(k_1 < 0\)
- 핀쿠션 왜곡(pincushion): 보통 \(k_1 > 0\)

### 2.2. Tangential Distortion
렌즈와 센서가 완벽히 정렬되지 않았을 때 생기는 왜곡.

$$
\begin{aligned}
 x_\text{tan} &= x + [2 p_1 x y + p_2 (r^2 + 2 x^2)] \\
 y_\text{tan} &= y + [p_1 (r^2 + 2 y^2) + 2 p_2 x y]
\end{aligned}
$$

- \(p_1, p_2\): tangential distortion 계수

### 2.3. Brown–Conrady 전체 모델
정규화 좌표 \((x, y)\) 에 대해 radial + tangential을 합치면:

$$
\begin{aligned}
 x' &= x (1 + k_1 r^2 + k_2 r^4 + k_3 r^6) + 2 p_1 x y + p_2 (r^2 + 2 x^2) \\
 y' &= y (1 + k_1 r^2 + k_2 r^4 + k_3 r^6) + p_1 (r^2 + 2 y^2) + 2 p_2 x y
\end{aligned}
$$

이 \((x', y')\)가 왜곡된 정규화 좌표이며, 이후 Intrinsic 행렬 \(K\)를 곱해 픽셀 좌표로 간다.

---

## 3. OpenCV Fisheye Model (Kannala–Brandt)
초광각 / fisheye 렌즈에서 Brown–Conrady 모델은 정확도가 떨어질 수 있다.
그래서 각도 기반으로 왜곡을 모델링하는 Kannala–Brandt 계열을 사용한다.

OpenCV의 `cv::fisheye` 모듈에서 사용하는 모델 개념 요약:

1. 3D 점 \(X_c = (X, Y, Z)\)를 카메라 좌표계에서 단위 방향 벡터로 정규화
   $$
   x = X / Z, \quad y = Y / Z
   $$

2. 광축과의 각도 \(\theta\) 계산
   $$
   r = \sqrt{x^2 + y^2}, \quad \theta = \arctan(r)
   $$

3. 왜곡된 반지름 \(r_d\)를 각도의 다항식으로 표현
   $$
   \theta_d = \theta + k_1 \theta^3 + k_2 \theta^5 + k_3 \theta^7 + k_4 \theta^9
   $$

4. 정규화 평면상의 좌표로 변환
   $$
   x_d = x \cdot \frac{\theta_d}{r}, \quad y_d = y \cdot \frac{\theta_d}{r}
   $$

5. Intrinsic 행렬 \(K\)로 픽셀 좌표로 변환
   $$
   \begin{bmatrix} u \\ v \\ 1 \end{bmatrix} = K \begin{bmatrix} x_d \\ y_d \\ 1 \end{bmatrix}
   $$

### 3.1. 특징 요약
- Brown–Conrady: \(r\) (거리)에 대한 다항식 → 일반 렌즈에 적합
- Kannala–Brandt: \(\theta\) (각도)에 대한 다항식 → 초광각, fisheye에 더 자연스러운 모델
- OpenCV에서는 `cv::fisheye` 네임스페이스로 분리하여 사용

---

## 4. 정리 및 한 줄 요약
- Brown–Conrady: "일반적인 pinhole 카메라 왜곡 모델 (radial + tangential 다항식)"
- Kannala–Brandt (OpenCV Fisheye): "광각·fisheye 렌즈용, 각도 기반 다항식 왜곡 모델"

Surround View에서는 특히 차량 주변을 넓게 담기 위해 fisheye 렌즈를 쓰므로,
실제 시스템 구현 시에는 OpenCV의 `fisheye` 모델을 사용해 undistort를 수행하는 것이 일반적이다.