# sketchflow
Sketchflow is an automatic sketch inbetweening tool that efficiently generates intermediate frames between key drawings. Implemented in C++, its core algorithm leverages an enhanced Scale-Invariant Feature Transform method for rapid keyframe registration, establishing high-precision feature point correspondences.To address deformation artifacts in traditional inbetweening, Sketchflow optimizes the "as-rigid-as-possible" morphing framework by introducing local deformation weighting factors, significantly improving the naturalness and fidelity of animated transitions.

Sketchflow是一个自动生成草图中间帧的软件，基于C++实现了一套高效的图像中间帧自动生成方案，核心采用改进的SIFT（尺度不变特征变换）算法进行关键帧快速配准，通过特征点匹配建立高精度对应关系。针对传统中间帧生成存在的形变失真问题，优化了"尽可能刚性"算法框架，引入局部变形权重因子，提升了动画过渡的自然度与保真度。

The code is available on the master branch.

切换至master分支查看代码。
