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

![para1.png](https://raw.githubusercontent.com/mboath/MyTinyRenderer/main/images/readme_pic/para1.png)

![para2.png](https://raw.githubusercontent.com/mboath/MyTinyRenderer/main/images/readme_pic/para2.png)

![para3.png](https://raw.githubusercontent.com/mboath/MyTinyRenderer/main/images/readme_pic/para3.png)

---

## 2. 光栅化

![para4.png](https://raw.githubusercontent.com/mboath/MyTinyRenderer/main/images/readme_pic/para4.png)

---

## 3. 片元着色器

* 着色方式：Phong Shading

* 光照模型：Blinn-Phong

  ![Blinn-Phong.png](https://raw.githubusercontent.com/mboath/MyTinyRenderer/main/images/readme_pic/Blinn-Phong.png)


![para5.png](https://raw.githubusercontent.com/mboath/MyTinyRenderer/main/images/readme_pic/para5.png)

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