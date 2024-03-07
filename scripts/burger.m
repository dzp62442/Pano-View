close;clc;clear;

% ����ģ�Ͳ���
R = 2; % ��Բ�İ뾶
d = 1; % СԲ��ֱ��
hole_rad = 0.1; % Բ�����Ŀն��뾶
max_size_vert = 50; % �����С

% ���Ƶײ���ͬ��ԲԲ��
r_disk = linspace(hole_rad, R - d, max_size_vert/4); % Բ�̰뾶�������Ŀն�����뾶
theta = linspace(0, 2 * pi, max_size_vert); % �Ƕ�
[R_disk, THETA_disk] = meshgrid(r_disk, theta);
X_disk = R_disk .* cos(THETA_disk);
Z_disk = R_disk .* sin(THETA_disk);
Y_disk = zeros(size(X_disk)); % Բ����������ײ�
mesh(X_disk, Y_disk, Z_disk, 'CData', Y_disk, 'FaceAlpha', 0.7,'LineWidth', 1.5);
hold on;

% ���Ƶײ�С����
t1 = linspace(0, d, max_size_vert/4); % ������뾶
theta = linspace(0, 2 * pi, max_size_vert); % �Ƕ�
[T1, THETA] = meshgrid(t1, theta);
r = sqrt(d^2 - (d - T1).^2) + R - d;
X1 = r .* cos(THETA);
Z1 = r .* sin(THETA);
Y1 = T1;
mesh(X1, Y1, Z1, 'CData', Y1, 'FaceAlpha', 0.7,'LineWidth', 1.5);
hold on;

% ���ƶ���������
t2 = linspace(d, d+R, max_size_vert/2); % ������뾶
theta = linspace(0, 2 * pi, max_size_vert); % �Ƕ�
[T2, THETA] = meshgrid(t2, theta);
r = sqrt(R^2 - (T2 - d).^2);
X2 = r .* cos(THETA);
Z2 = r .* sin(THETA);
Y2 = T2;
mesh(X2, Y2, Z2, 'CData', Y2, 'FaceAlpha', 0.7,'LineWidth', 1.5);
hold on;

% ��ʾЧ��
axis equal;
shading interp; % ��ֵ��ɫ��ʹ��ɫ����������ƽ������
grid on; % ����������
% xlabel('X'); ylabel('Y'); zlabel('Z');
% title('Drop-Shaped Mesh with Parameters');
axis off;
view(180, -80);
