# MyTinyRenderer 软光栅化渲染器

* 语言：C++

* 环境：Windows 10, Microsoft Visual Studio 2019

* 运行方式：

  ```
  MyTinyRenderer.exe obj文件1路径 obj文件2路径 ...
  ```

![african_head_2.jpg](https://raw.githubusercontent.com/mboath/MyTinyRenderer/main/images/african_head_2.jpg)

---

## 1. 顶点着色器

* 默认模型位于世界坐标系原点，忽略模型空间到世界空间的变换

* View Transformation 世界空间 -> 观察空间：摄像机位置（$\vec{e}$）在原点，朝向（$\hat{g}$）指向-z，上方向（$\hat{t}$）指向y
  $$
  M_{view} = R_{view}T_{view}
  \\
  T_{view}=
  \left[\begin{matrix}
  1 & 0 & 0 & -x_e\\
  0 & 1 & 0 & -y_e\\
  0 & 0 & 1 & -z_e\\
  0 & 0 & 0 & 1
  \end{matrix}\right]
  \\
  R_{view}=
  \left[\begin{matrix}
  x_{\hat{g}×\hat{t}} & y_{\hat{g}×\hat{t}} & z_{\hat{g}×\hat{t}} & 0\\
  x_t & y_t & z_t & 0\\
  x_{-g} & y_{-g} & z_{-g} & 0\\
  0 & 0 & 0 & 1
  \end{matrix}\right]
  $$

* Projection Transformation 观察空间 -> 裁剪空间：近平面n，远平面f，$b=-t$，$l=-r$
  $$
  M_{persp->ortho}=
  \left[\begin{matrix}
  n & 0 & 0 & 0\\
  0 & n & 0 & 0\\
  0 & 0 & n + f & -nf\\
  0 & 0 & 1 & 0
  \end{matrix}\right]
  \\
  M_{ortho}=
  \left[\begin{matrix}
  \frac{2}{r-l} & 0 & 0 & 0\\
  0 & \frac{2}{t-b} & 0 & 0\\
  0 & 0 & \frac{2}{n-f} & 0\\
  0 & 0 & 0 & 1
  \end{matrix}\right]
  *
  \left[\begin{matrix}
  1 & 0 & 0 & -\frac{r+l}{2}\\
  0 & 1 & 0 & -\frac{t+b}{2}\\
  0 & 0 & 1 & -\frac{n+f}{2}\\
  0 & 0 & 0 & 1
  \end{matrix}\right]
  =
  \left[\begin{matrix}
  \frac{2}{r-l} & 0 & 0 & -\frac{r+l}{r-l}\\
  0 & \frac{2}{t-b} & 0 & -\frac{t+b}{t-b}\\
  0 & 0 & \frac{2}{n-f} & -\frac{n+f}{n-f}\\
  0 & 0 & 0 & 1
  \end{matrix}\right]
  \\
  M_{persp}=M_{ortho}*M_{persp->ortho}=
  \left[\begin{matrix}
  \frac{2n}{r-l} & 0 & -\frac{r+l}{r-l} & 0\\
  0 & \frac{2n}{t-b} & -\frac{t+b}{t-b} & 0\\
  0 & 0 & \frac{n+f}{n-f} & -\frac{2nf}{n-f}\\
  0 & 0 & 1 & 0
  \end{matrix}\right]
  =
  \left[\begin{matrix}
  \frac{n}{r} & 0 & 0 & 0\\
  0 & \frac{n}{t} & 0 & 0\\
  0 & 0 & \frac{n+f}{n-f} & -\frac{2nf}{n-f}\\
  0 & 0 & 1 & 0
  \end{matrix}\right]
  $$
  其中$t=|n|·tan\frac{fovY}{2}$，$r=aspect·t$

* Perspective Division 裁剪空间 -> 标准化设备空间

  注：保留w分量以便做透视修正

* Viewport Tranformation 标准化设备空间 -> 屏幕空间
  $$
  M_{viewport}=
  \left[\begin{matrix}
  \frac{width}{2} & 0 & 0 & x+\frac{width}{2}\\
  0 & \frac{height}{2} & 0 & y+\frac{height}{2}\\
  0 & 0 & 1 & 0\\
  0 & 0 & 0 & 1
  \end{matrix}\right]
  $$

---

## 2. 光栅化

* 确定AABB包围盒

* 对包围盒内的每个pixel求重心坐标$(\alpha',\beta',\gamma')$，跳过三角形外的pixel（任一分量小于0）

* 透视修正

  透视修正后的深度插值公式：$\frac{1}{Z}=\frac{\alpha'}{Z_A}+\frac{\beta'}{Z_B}+\frac{\gamma'}{Z_C}$

  透视修正后的任意属性差值公式：$I=Z·(\alpha'\frac{I_A}{Z_A}+\beta'\frac{I_B}{Z_B}+\gamma'\frac{I_C}{Z_C})=\alpha I_A+\beta I_B+\gamma I_C$

  其中$\alpha=Z·\frac{\alpha'}{Z_A}$，$\beta=Z·\frac{\beta'}{Z_B}$，$\gamma=Z·\frac{\gamma'}{Z_C}$

  $Z_A$、$Z_B$、$Z_C$代入透视变换后顶点的w分量（透视变换前的z分量）即可

* 深度测试（Early-Z）

---

## 3. 片元着色器

* 着色方式：Phong Shading

* 光照模型：Blinn-Phong

  ![Blinn-Phont.png](https://raw.githubusercontent.com/mboath/MyTinyRenderer/main/images/Blinn-Phong.png)

  其中$k_a$和$k_d$为由插值得到的uv坐标从纹理图上取得的颜色（可选邻近插值或双线性插值），其他参数可调

* 硬阴影的实现——Shadow Mapping
  * DepthShader：顶点着色器进行世界坐标到屏幕坐标的变换，片元着色器返回白色即可
  * 第一层渲染：将摄像机置于光源处，朝向模型，渲染深度图过程中更新shadow buffer
  * 第二层渲染：从摄像机渲染，对插值得到的世界坐标计算到光源摄像机的深度以及屏幕坐标，与shadow buffer中的深度值进行比较，若深度小于shadow buffer中的深度值，则位于阴影区，对位于阴影区的片元，减弱diffuse和specular分量
  * 存在问题
    - [x] z-fighting：添加偏移量 -> 阴影缺失 
    - [ ] 锯齿

---

## Results

* 变换视角

  ![african_head_camera.jpg](https://raw.githubusercontent.com/mboath/MyTinyRenderer/main/images/african_head_camera.jpg)

* 变换光照角度

  ![african_head_light.jpg](https://raw.githubusercontent.com/mboath/MyTinyRenderer/main/images/african_head_light.jpg)

* 其他模型

  ![boggie.jpg](https://raw.githubusercontent.com/mboath/MyTinyRenderer/main/images/boggie.jpg)

  ![diablo3_pose.jpg](https://raw.githubusercontent.com/mboath/MyTinyRenderer/main/images/diablo3_pose.jpg)

---

## References

[1] https://github.com/ssloy/tinyrenderer/wiki

[2] GAMES101-现代计算机图形学入门-闫令琪