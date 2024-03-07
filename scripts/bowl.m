close;clc;clear;

% 参数定义
param_a = 0.4; % 抛物面参数
param_b = 0.4;
param_c = 0.2;
rad = 0.7; % 抛物面顶部半径
inner_rad = 0.4; % 抛物面底部半径和圆盘外半径
hole_rad = 0.02; % 圆盘中心空洞半径
max_size_vert = 50; % 网格大小
polar_coord = 2 * pi; % 极坐标全范围

% 绘制碗状网格 - 抛物面
r_parabola = linspace(inner_rad, rad, max_size_vert/2); % 抛物面半径
theta = linspace(0, polar_coord, max_size_vert); % 角度
[R_parabola, THETA] = meshgrid(r_parabola, theta);
X_parabola = R_parabola .* cos(THETA);
Z_parabola = R_parabola .* sin(THETA);
Y_parabola = param_c * ((X_parabola / param_a).^2 + (Z_parabola / param_b).^2);
mesh(X_parabola, Y_parabola, Z_parabola, 'CData', Y_parabola, 'FaceAlpha', 0.7,'LineWidth', 1.5);
hold on;

% 绘制底部的同心圆圆盘
r_disk = linspace(hole_rad, inner_rad, max_size_vert/2); % 圆盘半径，从中心空洞到外半径
[R_disk, THETA_disk] = meshgrid(r_disk, theta);
X_disk = R_disk .* cos(THETA_disk);
Z_disk = R_disk .* sin(THETA_disk);
Y_disk = zeros(size(X_disk)) + min(Y_parabola(:)); % 圆盘在抛物面底部
mesh(X_disk, Y_disk, Z_disk, 'CData', Y_disk, 'FaceAlpha', 1,'LineWidth', 1.5);
hold on;

% 显示效果
axis equal;
shading interp; % 插值着色，使颜色在网格面上平滑过渡
xlabel('X'); ylabel('Y'); zlabel('Z');
% title('Bowl-Shaped Mesh with Parameters');
axis off;
view(180, -60);


