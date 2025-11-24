# Camera Matrix 정리

## 1. Intrinsic Matrix (카메라 내부 파라미터)
- 이미지 센서 좌표계에서 픽셀 좌표계로 변환하는 역할.
- 초점 거리와 주점(principal point)을 포함.

일반적인 형태:

$$
K = \begin{bmatrix}
 f_x & s   & c_x \\
 0   & f_y & c_y \\
 0   & 0   & 1
\end{bmatrix}
$$

- $f_x, f_y$: x, y 방향 초점 거리 (픽셀 단위)
- $s$: skew (대부분 0으로 가정)
- $c_x, c_y$: 주점(이미지 중심 근처)

## 2. Extrinsic Matrix (카메라 외부 파라미터)
- 월드 좌표계와 카메라 좌표계를 연결하는 변환.
- 회전 행렬 $R$과 평행이동 벡터 $t$로 구성.

월드 좌표 $X_w = [X\ Y\ Z\ 1]^T$ 가 카메라 좌표 $X_c = [X_c\ Y_c\ Z_c]^T$ 로 변환될 때:

$$
X_c = R X_w + t
$$

동차 좌표계로 쓰면:

$$
\begin{bmatrix}
X_c \\
Y_c \\
Z_c \\
1
\end{bmatrix}
=
\begin{bmatrix}
R & t \\
0 & 1
\end{bmatrix}
\begin{bmatrix}
X \\
Y \\
Z \\
1
\end{bmatrix}
$$

여기서 $R$은 $3\times3$ 회전 행렬, $t$는 $3\times1$ translation 벡터.

## 3. Projection Matrix 구성 방식
- 3D 월드 좌표를 2D 이미지 평면으로 사영(projection)하는 전체 변환.
- 기본 형태: $P = K [R \ | \ t]$ (크기: $3 \times 4$)

월드 좌표 $X_w = [X\ Y\ Z\ 1]^T$ 를 이미지 동차 좌표 $x = [u\ v\ 1]^T$ 로 보낼 때:

1. 카메라 좌표계로 변환:
   $$ X_c = R X_w + t $$

2. 카메라 좌표를 정규화된 이미지 평면으로 투영:
   $$
   x_n = \begin{bmatrix}
   x_n \\
   y_n \\
   1
   \end{bmatrix}
   =
   \begin{bmatrix}
   X_c / Z_c \\
   Y_c / Z_c \\
   1
   \end{bmatrix}
   $$

3. Intrinsic matrix를 적용해 픽셀 좌표로 변환:
   $$
   x = K x_n
   $$

이를 한 번에 쓰면:

$$
\lambda
\begin{bmatrix}
 u \\
 v \\
 1
\end{bmatrix}
=
P
\begin{bmatrix}
 X \\
 Y \\
 Z \\
 1
\end{bmatrix},
\quad
P = K [R \ | \ t]
$$

여기서 $\lambda$는 동차 좌표의 스케일 팩터.

---

- Intrinsic: 카메라 "안의" 설정 (렌즈, 센서)
- Extrinsic: 카메라가 월드에서 어디에 어떻게 놓였는지 (pose)
- Projection Matrix: 이 둘을 곱해서 3D → 2D 전체 변환을 표현한 행렬.

## 4. Homography 개념
- 한 평면 위의 점들이 서로 다른 두 뷰(또는 이미지) 사이에서 맵핑될 때 사용되는 $3\times3$ 행렬.
- 동차 좌표계에서 정의되며, 스케일을 제외하고 8개의 자유도를 가짐.

한 이미지 평면의 점을 $x = [u\ v\ 1]^T$, 다른 평면(또는 다른 이미지) 상의 대응점을 $x' = [u'\ v'\ 1]^T$ 라 하면:

$$
\lambda x' = H x, \quad
H = \begin{bmatrix}
h_{11} & h_{12} & h_{13} \\
h_{21} & h_{22} & h_{23} \\
h_{31} & h_{32} & h_{33}
\end{bmatrix}
$$

\(H\)는 임의의 스케일 팩터까지 정의되므로, 실제로는 8개의 독립 파라미터를 갖는다.

### 4.1. 카메라 투영과 평면
카메라 Projection Matrix가 \(P = K [R \ | \ t]\)이고, 월드 좌표계에서 어떤 평면이 다음과 같이 주어진다고 하자:

$$
n^T X + d = 0
$$

- \(n\): 평면의 법선 벡터 (3D)
- \(d\): 평면까지의 거리(스케일 포함)

이 평면 위의 점들만 고려하면, 3D 점을 평면 좌표계로 parameterize해서 2D ↔ 2D 관계(즉 homography)로 표현할 수 있다.

---

## 5. Ground-plane Mapping 수학
Surround View에서는 바닥(ground plane)을 하나의 평면으로 가정하고,
카메라 이미지 상의 픽셀을 이 바닥 평면 위의 좌표로 보낸다.

### 5.1. 카메라에서 Ground Plane으로의 Homography
Ground plane이 월드 좌표계에서 \(Z = 0\) (또는 \(Y = 0\))과 같이 하나의 평면으로 정의된다고 가정하자.

예를 들어, 월드 좌표계를 \((X, Y, Z)\)라 두고, Ground plane을 \(Z = 0\)이라 하면:

- Ground 평면 위의 점: \(X_g = [X\ Y\ 0\ 1]^T\)
- 카메라 Projection: \(P = K [R \ | \ t]\)

따라서:

$$
\lambda x = P X_g = K [R \ | \ t]
\begin{bmatrix}
X \\
Y \\
0 \\
1
\end{bmatrix}
= K (r_1 X + r_2 Y + t)
$$

여기서 \(r_1, r_2\)는 회전 행렬 \(R\)의 첫 두 열이다.

이를 정리하면:

$$
\lambda x = H_g
\begin{bmatrix}
X \\
Y \\
1
\end{bmatrix}, \quad
H_g = K [r_1\ r_2\ t]
$$

- \(H_g\): Ground plane (월드 2D 좌표) → 이미지 좌표로 가는 homography
- 역행렬 \(H_g^{-1}\): 이미지 좌표 → Ground plane 좌표로의 mapping

실제 구현에서는 보통 이미지의 픽셀 좌표 \((u, v)\)를 homography 역변환을 이용해 Ground plane상의 \((X, Y)\)로 매핑한 뒤,
원하는 top-view(버드아이 뷰) 이미지 격자로 다시 샘플링한다.

### 5.2. OpenCV에서의 Homography 활용
- `cv::findHomography`를 이용해 대응점(이미지 ↔ 평면)을 기반으로 \(H\)를 추정
- `cv::warpPerspective`를 이용해 homography를 적용하여 이미지 전체를 변환

Surround View에서는 각 카메라에 대해 "이미지 → Ground plane(top-view)" homography를 구한 다음,
동일한 Ground plane 상에 투영된 네 장의 이미지를 겹쳐(blend) 360° Surround View를 구성한다.