close;clc;clear;

% 汉堡模型参数
R = 2; % 大圆的半径
d = 1; % 小圆的直径
hole_rad = 0.1; % 圆盘中心空洞半径
max_size_vert = 50; % 网格大小

% 绘制底部的同心圆圆盘
r_disk = linspace(hole_rad, R - d, max_size_vert/4); % 圆盘半径，从中心空洞到外半径
theta = linspace(0, 2 * pi, max_size_vert); % 角度
[R_disk, THETA_disk] = meshgrid(r_disk, theta);
X_disk = R_disk .* cos(THETA_disk);
Z_disk = R_disk .* sin(THETA_disk);
Y_disk = zeros(size(X_disk)); % 圆盘在抛物面底部
mesh(X_disk, Y_disk, Z_disk, 'CData', Y_disk, 'FaceAlpha', 0.7,'LineWidth', 1.5);
hold on;

% 绘制底部小球面
t1 = linspace(0, d, max_size_vert/4); % 抛物面半径
theta = linspace(0, 2 * pi, max_size_vert); % 角度
[T1, THETA] = meshgrid(t1, theta);
r = sqrt(d^2 - (d - T1).^2) + R - d;
X1 = r .* cos(THETA);
Z1 = r .* sin(THETA);
Y1 = T1;
mesh(X1, Y1, Z1, 'CData', Y1, 'FaceAlpha', 0.7,'LineWidth', 1.5);
hold on;

% 绘制顶部大球面
t2 = linspace(d, d+R, max_size_vert/2); % 抛物面半径
theta = linspace(0, 2 * pi, max_size_vert); % 角度
[T2, THETA] = meshgrid(t2, theta);
r = sqrt(R^2 - (T2 - d).^2);
X2 = r .* cos(THETA);
Z2 = r .* sin(THETA);
Y2 = T2;
mesh(X2, Y2, Z2, 'CData', Y2, 'FaceAlpha', 0.7,'LineWidth', 1.5);
hold on;

% 显示效果
axis equal;
shading interp; % 插值着色，使颜色在网格面上平滑过渡
grid on; % 开启网格线
% xlabel('X'); ylabel('Y'); zlabel('Z');
% title('Drop-Shaped Mesh with Parameters');
axis off;
view(180, -80);
