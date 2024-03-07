close; clear; clc;

% 参数定义
resolution = 100; % 网格分辨率
ya = 3;
yb = 2;
yc = 6;
yd = 5;
ra = 3;
rb = 5;
rc = 4;
% 参数1：3 2 5 4 3 5 4
% 参数2：3 3 6 5 4 6 4
% 参数3：3 2 6 5 3 5 4

% 使用meshgrid生成参数网格
theta = linspace(0, 2*pi, resolution); % 水平方向的参数（围绕Y轴旋转）
t = linspace(0, pi, resolution); % 竖直方向的参数（定义水滴高度）
[T, Theta] = meshgrid(t, theta);

% 绘制水滴形状
y1 = ya * (1 + cos(T)) .* (yb + cos(T)) ./ (yc + yd * cos(T));
r = ra * (1 + cos(T)) .* sin(T) ./ (rb + rc * cos(T));
X = r .* cos(Theta);
Z = r .* sin(Theta);
Y = - y1;
mesh(X, Y, Z, 'CData', Y,  'FaceAlpha', 0.5,'LineWidth', 1.5);
hold on;

% 显示效果
axis equal;
shading interp; % 插值着色，使颜色在网格面上平滑过渡
grid on; % 开启网格线
xlabel('X'); ylabel('Y'); zlabel('Z');
% title('Drop-Shaped Mesh with Parameters');
% axis off;
view(180, -80);


%% 曲线分析
% 参数方程
T = linspace(0, 2*pi, resolution);  % [0, 2*pi] 为完整曲线
x = ra * (1 + cos(T)) .* sin(T) ./ (rb + rc * cos(T));
y = - ya * (1 + cos(T)) .* (yb + cos(T)) ./ (yc + yd * cos(T));

% 绘制参数方程定义的曲线
figure;
plot(x, y, 'LineWidth', 2);
axis equal;
grid on; % 开启网格
xlabel('x'); % x轴标签
ylabel('y'); % y轴标签
% title('Parametric Curve'); % 标题
% axis off;
