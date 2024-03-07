close; clear; clc;

% ��������
resolution = 100; % ����ֱ���
ya = 3;
yb = 2;
yc = 6;
yd = 5;
ra = 3;
rb = 5;
rc = 4;
% ����1��3 2 5 4 3 5 4
% ����2��3 3 6 5 4 6 4
% ����3��3 2 6 5 3 5 4

% ʹ��meshgrid���ɲ�������
theta = linspace(0, 2*pi, resolution); % ˮƽ����Ĳ�����Χ��Y����ת��
t = linspace(0, pi, resolution); % ��ֱ����Ĳ���������ˮ�θ߶ȣ�
[T, Theta] = meshgrid(t, theta);

% ����ˮ����״
y1 = ya * (1 + cos(T)) .* (yb + cos(T)) ./ (yc + yd * cos(T));
r = ra * (1 + cos(T)) .* sin(T) ./ (rb + rc * cos(T));
X = r .* cos(Theta);
Z = r .* sin(Theta);
Y = - y1;
mesh(X, Y, Z, 'CData', Y,  'FaceAlpha', 0.5,'LineWidth', 1.5);
hold on;

% ��ʾЧ��
axis equal;
shading interp; % ��ֵ��ɫ��ʹ��ɫ����������ƽ������
grid on; % ����������
xlabel('X'); ylabel('Y'); zlabel('Z');
% title('Drop-Shaped Mesh with Parameters');
% axis off;
view(180, -80);


%% ���߷���
% ��������
T = linspace(0, 2*pi, resolution);  % [0, 2*pi] Ϊ��������
x = ra * (1 + cos(T)) .* sin(T) ./ (rb + rc * cos(T));
y = - ya * (1 + cos(T)) .* (yb + cos(T)) ./ (yc + yd * cos(T));

% ���Ʋ������̶��������
figure;
plot(x, y, 'LineWidth', 2);
axis equal;
grid on; % ��������
xlabel('x'); % x���ǩ
ylabel('y'); % y���ǩ
% title('Parametric Curve'); % ����
% axis off;
