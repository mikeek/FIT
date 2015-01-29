I = imread('xkozub03.bmp');

% zaostreni
h = [-0.5 -0.5 -0.5; -0.5 5 -0.5; -0.5 -0.5 -0.5];
I1 = imfilter(I,h);
imwrite(I1, 'step1.bmp');

% otoceni
I2 = fliplr(I1);
imwrite(I2, 'step2.bmp');

% medianovy filtr
I3 = medfilt2(I2, [5 5]);
imwrite(I3, 'step3.bmp');

% rozmazani
h4 = [1 1 1 1 1; 1 3 3 3 1; 1 3 9 3 1; 1 3 3 3 1; 1 1 1 1 1] / 49;
I4 = imfilter(I3, h4);
imwrite(I4, 'step4.bmp');

% chyba v obraze
noise = 0;
I4tmp = im2double(I4tmp);
Itmp = im2double(fliplr(I));
for (x=1:512)
    for (y=1:512)
        noise = noise + double(abs(Itmp(x, y) - I4tmp(x, y)));
    end;
end;
noise = noise/512/512

% roztazeni histogramu
I5d = im2double(I4);

inLo = min(min(I5d));
inHi = max(max(I5d));

I5 = imadjust(I4, [inLo inHi], [0 1]);
imwrite(I5, 'step5.bmp');

% stredni hodnota a smerodatna odchylka
mean_no_hist = mean2(im2double(I4))
std_no_hist = std2(im2double(I4))
mean_hist = mean2(im2double(I5))
std_hist = std2(im2double(I5))

% kvantizace obrazu

a = 0;
b = 255;
N = 2;
I5d = double(I5);
I6 = zeros(512, 512);
for k=1:512
    for l=1:512
        I6(k, l) = round(((2^N)-1)*(I5d(k, l)-a)/(b-a))*(b-a)/((2^N)-1) + a;
    end
end

I6 = uint8(I6);
imwrite(I6, 'step6.bmp');
