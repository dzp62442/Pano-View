close;clc;clear;

% ��������
param_a = 0.4; % ���������
param_b = 0.4;
param_c = 0.2;
rad = 0.7; % �����涥���뾶
inner_rad = 0.4; % ������ײ��뾶��Բ����뾶
hole_rad = 0.02; % Բ�����Ŀն��뾶
max_size_vert = 50; % �����С
polar_coord = 2 * pi; % ������ȫ��Χ

% ������״���� - ������
r_parabola = linspace(inner_rad, rad, max_size_vert/2); % ������뾶
theta = linspace(0, polar_coord, max_size_vert); % �Ƕ�
[R_parabola, THETA] = meshgrid(r_parabola, theta);
X_parabola = R_parabola .* cos(THETA);
Z_parabola = R_parabola .* sin(THETA);
Y_parabola = param_c * ((X_parabola / param_a).^2 + (Z_parabola / param_b).^2);
mesh(X_parabola, Y_parabola, Z_parabola, 'CData', Y_parabola, 'FaceAlpha', 0.7,'LineWidth', 1.5);
hold on;

% ���Ƶײ���ͬ��ԲԲ��
r_disk = linspace(hole_rad, inner_rad, max_size_vert/2); % Բ�̰뾶�������Ŀն�����뾶
[R_disk, THETA_disk] = meshgrid(r_disk, theta);
X_disk = R_disk .* cos(THETA_disk);
Z_disk = R_disk .* sin(THETA_disk);
Y_disk = zeros(size(X_disk)) + min(Y_parabola(:)); % Բ����������ײ�
mesh(X_disk, Y_disk, Z_disk, 'CData', Y_disk, 'FaceAlpha', 1,'LineWidth', 1.5);
hold on;

% ��ʾЧ��
axis equal;
shading interp; % ��ֵ��ɫ��ʹ��ɫ����������ƽ������
xlabel('X'); ylabel('Y'); zlabel('Z');
% title('Bowl-Shaped Mesh with Parameters');
axis off;
view(180, -60);


