# KAIST - SoC RobotWar
## 출전자격 TEST 대비 영상기초
작성자 : DH Kim
## 설명1

1. 영상은 기본적으로 배열이나 포인터를 이용해서 저장합니다.

2. 색상을 표현하는 모델에는 여러가지 방법이 있습니다.
해당 소스에서는 YUV를 이용하여 영상데이터를 lpVHdr->lpData에 저장합니다(편의를 위해 RGB로 변환하는 예제소스를 추가하였습니다)

 2-1. 0(검정), 1(흰색)으로만 구성된 binary 이미지

 2-2. 0~255의 값으로 이루어진 gray 모델(흔히 말하는 흑백영상이 gray색상모델입니다)

 2-3. 빨강(255,0,0), 초록(0,255,0) 파랑(0,0,255)으로 구성된 RGB모델

 2-4.  Hue(색조), Saturation(채도), Value(명도)로 구성된 HSV모델

 2-5. 밝기성분과 색차정보로 구성된 YUV(YCbCr)모델

---

## 과제1

1. RGB로 구성된 영상을 gray영상으로 변환하세요
> gray = (R + G + B)/3 을 이용하시면 됩니다.

2. YUV(YCbCr)로 구성된 영상을 gray scale로 변환하세요

---

## 설명2
### [영상이진화(Threshold)](http://homepages.inf.ed.ac.uk/rbf/HIPR2/threshld.htm)
영상 이진화는 color 혹은 gray scale로 이루어진 이미지를 0(검정), 1(흰색)(혹은 255)으로 변환시키는 것을 의미합니다.
임계값 T(밝기)를 기준으로 즉 T보다 어두운 픽셀은 검정으로, T보다 밝은 픽셀은 흰색으로 변환합니다.

하지만 영상에서 최적의 T값은 계속 변하게 됩니다. 이러한 T값을 자동으로 찾는 방법으로는 전역고정이진화, 지역가변 이진화, Hysteresis thresholding등이 있습니다.

<strong>1. 전역고정이진화</strong>

전역고정이진화는 한 이미지에 대한 임계값을 하나로 고정하여 threshold를 진행합니다. 이때 T값을 자동으로 찾는 방법으로는 Otsu's method가 있습니다.
어두운 픽셀들의 비율을 `α`, 밝기 평균을 `μ1`, 분산을 `σ1^2`, T보다 밝은 픽셀들의 비율을 `β`, 밝기 평균을 `μ2`, 분산을 `σ2^2`라 했을 때 (α+β=1), intra-class 분산과 inter-class 분산은 각각 다음과 같습니다.
```
intra-class variance = ασ1^2+βσ2^2     --- (1)

inter-class variance = αβ(μ1-μ2)^2     --- (2)
```
(1)을 최소화하는것은 (2)를 최대로 만드는것과 동일합니다.

<strong>2. 지역가변이진화</strong>

밝기가 급격하게 변하는 이미지에서는 전역고정이진화를 적용했을때 원하는 임계값이 검출이 되지 않습니다. 이러한 경우에는 각 픽셀마다 T값을 다르게 지정하는 가변 threshold를 진행해야 합니다.
지역 가변 이진화 알고리즘으로는 여러가지 방법이 있지만, 이 글에서는 대표적으로 1가지 알고리즘을 소개합니다.

OpenCV에서 adaptiveThreshold라는 이름으로 제공하는 함수에서는 다음과 같은 방법을 사용합니다.
영상의 각 픽셀마다 서로 다른 threshold를 사용하며 임계값 T는 해당 픽셀을 중심으로 n*n주변 영역의 밝기 평균에 상수 C를 빼서 결정합니다.
<img src="http://cfile30.uf.tistory.com/image/2120CA3B52CB6585089529" />

주변 영역의 크기와 상수 C의 값에 따라 서로 다른 결과가 나타날 수 있으며, 이미지의 특징에 따라 적절히 값을 조절하면 좋은 결과를 얻을 수 있습니다.

[참고자료](http://darkpgmr.tistory.com/115)

---

## 과제 2

1.  Otsu's method를 이용한 전역고정이진화를 구현하시오
>Hint : 반복문을 이용하여 T를 0부터 255까지 검사하며 (1)을 최소 혹은 (2)를 최대를 만족하게 만드는 T값을 검출하고 이 값을 이용하여 이진화를 진행하면 됩니다.

2. 한 픽셀을 중심으로 N*N영역의 밝기평균을 이용한 지역가변이진화를 구현하시오(상수 C의 값은 본인이 원하는대로 사용하여도 무관함)

---

## 설명 3
자연조명과 인공조명은 영상을 처리하는데에 있어서 중요하게 고려해야할 요소입니다. 원하지 않는 조명이 포함되거나 약한 조명의 세기로 인해 영상을 구별하는데 문제가 발생하기 때문입니다. 
이미지의 모든 픽셀에 대한 명암(밝기)의 분포를 나타낸 값인 히스토그램은 이와 같은 상황에서 간단하면서도 유용하게 사용할 수 있는 도구입니다. 영상의 히스토그램을 분석하면 한쪽으로 치우쳐진 명도를 고르게 분포시키는 평활화 작업을 할 수 있습니다.

히스토그램을 이용해 명암값을 정규화하는 과정은 다음과 같습니다

1. 명암값의 빈도수에 대해 누적합을 계산합니다.
2. 각각의 누적합에 Scale Factor(최대명암값/전체 픽셀 수)를 곱합니다.
3. 명암값을 (2)에서 구한 값으로 대체합니다.

#### 예시

<div>
<table style="border-collapse:collapse;border-spacing:0"><tr><th style="font-family:Arial, Helvetica, sans-serif !important;;font-size:14px;font-weight:normal;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;text-align:center">3</th><th style="font-family:Arial, Helvetica, sans-serif !important;;font-size:14px;font-weight:normal;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;text-align:center;vertical-align:top">4</th><th style="font-family:Arial, Helvetica, sans-serif !important;;font-size:14px;font-weight:normal;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;text-align:center;vertical-align:top">6</th></tr><tr><td style="font-family:Arial, Helvetica, sans-serif !important;;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;text-align:center">7</td><td style="font-family:Arial, Helvetica, sans-serif !important;;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;text-align:center;vertical-align:top">6</td><td style="font-family:Arial, Helvetica, sans-serif !important;;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;text-align:center;vertical-align:top">5</td></tr><tr><td style="font-family:Arial, Helvetica, sans-serif !important;;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;text-align:center">8</td><td style="font-family:Arial, Helvetica, sans-serif !important;;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;text-align:center;vertical-align:top">7</td><td style="font-family:Arial, Helvetica, sans-serif !important;;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;text-align:center;vertical-align:top">6</td></tr><tr><td style="font-family:Arial, Helvetica, sans-serif !important;;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;text-align:center">3</td><td style="font-family:Arial, Helvetica, sans-serif !important;;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;text-align:center;vertical-align:top">1</td><td style="font-family:Arial, Helvetica, sans-serif !important;;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;text-align:center;vertical-align:top">5</td></tr></table>
</div>
다음과 같은 명암을 가진 영상데이터가 있다고 가정할때 명암값에 대한 빈도수와 그의 누적합을 구합니다.
평활화를 위한 정규화 식은 (각각의 누적합) / <span style="color:blue;">(전체 픽셀 수)</span> * <span style="color:red;">(최대명암값)</span> 이며 예시로 주어진 영상데이터의 빈도수, 누적합, 정규화 값은 다음과 같습니다

<div style= "overflow:hidden;">
<div style=" float:left; margin:auto;">
<table style="border-collapse:collapse;border-spacing:0"><tr><th style="font-family:Arial, Helvetica, sans-serif !important;;font-size:14px;font-weight:bold;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;background-color:#c0c0c0;text-align:center">명도</th><th style="font-family:Arial, Helvetica, sans-serif !important;;font-size:14px;font-weight:bold;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;background-color:#c0c0c0;text-align:center">빈도수</th><th style="font-family:Arial, sans-serif;font-size:14px;font-weight:bold;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;background-color:#c0c0c0;text-align:center;vertical-align:top">누적합</th></tr><tr><td style="font-family:Arial, Helvetica, sans-serif !important;;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;text-align:center">1</td><td style="font-family:Arial, Helvetica, sans-serif !important;;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;text-align:center">1</td><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;text-align:center;vertical-align:top">1</td></tr><tr><td style="font-family:Arial, Helvetica, sans-serif !important;;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;text-align:center">2</td><td style="font-family:Arial, Helvetica, sans-serif !important;;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;text-align:center">0</td><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;text-align:center;vertical-align:top">1</td></tr><tr><td style="font-family:Arial, Helvetica, sans-serif !important;;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;text-align:center">3</td><td style="font-family:Arial, Helvetica, sans-serif !important;;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;text-align:center">2</td><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;text-align:center;vertical-align:top">3</td></tr><tr><td style="font-family:Arial, Helvetica, sans-serif !important;;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;text-align:center;vertical-align:top">4</td><td style="font-family:Arial, Helvetica, sans-serif !important;;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;text-align:center;vertical-align:top">1</td><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;text-align:center;vertical-align:top">4</td></tr><tr><td style="font-family:Arial, Helvetica, sans-serif !important;;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;text-align:center;vertical-align:top">5</td><td style="font-family:Arial, Helvetica, sans-serif !important;;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;text-align:center;vertical-align:top">2</td><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;text-align:center;vertical-align:top">6</td></tr><tr><td style="font-family:Arial, Helvetica, sans-serif !important;;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;text-align:center;vertical-align:top">6</td><td style="font-family:Arial, Helvetica, sans-serif !important;;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;text-align:center;vertical-align:top">3</td><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;text-align:center;vertical-align:top">9</td></tr><tr><td style="font-family:Arial, Helvetica, sans-serif !important;;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;text-align:center;vertical-align:top">7</td><td style="font-family:Arial, Helvetica, sans-serif !important;;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;text-align:center;vertical-align:top">2</td><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;text-align:center;vertical-align:top">11</td></tr><tr><td style="font-family:Arial, Helvetica, sans-serif !important;;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;font-weight:bold;color:#fe0000;text-align:center;vertical-align:top">8</td><td style="font-family:Arial, Helvetica, sans-serif !important;;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;text-align:center;vertical-align:top">1</td><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;font-weight:bold;color:#3531ff;text-align:center;vertical-align:top">12</td></tr></table>
</div>

<div style="float:left;margin-left:3em;">
<p></p>
</div>

<div style="margin:auto;padding:auto;float:left">
<table style="border-collapse:collapse;border-spacing:0"><tr><th style="font-family:Arial, Helvetica, sans-serif !important;;font-size:14px;font-weight:bold;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;background-color:#c0c0c0;text-align:center">정규화</th></tr><tr><td style="font-family:Arial, Helvetica, sans-serif !important;;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;text-align:center">0.66667</td></tr><tr><td style="font-family:Arial, Helvetica, sans-serif !important;;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;text-align:center">0.66667</td></tr><tr><td style="font-family:Arial, Helvetica, sans-serif !important;;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;text-align:center">2</td></tr><tr><td style="font-family:Arial, Helvetica, sans-serif !important;;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;text-align:center">2.66667</td></tr><tr><td style="font-family:Arial, Helvetica, sans-serif !important;;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;text-align:center">4</td></tr><tr><td style="font-family:Arial, Helvetica, sans-serif !important;;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;text-align:center">6</td></tr><tr><td style="font-family:Arial, Helvetica, sans-serif !important;;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;text-align:center">7.33333</td></tr><tr><td style="font-family:Arial, Helvetica, sans-serif !important;;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;text-align:center">8</td></tr></table>
</div>

<div style="float:left;margin-left:3em;">
<p></p>
</div>
</div>

변환 전
<table style="border-collapse:collapse;border-spacing:0"><tr><th style="font-family:Arial, Helvetica, sans-serif !important;;font-size:14px;font-weight:normal;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;text-align:center">3</th><th style="font-family:Arial, Helvetica, sans-serif !important;;font-size:14px;font-weight:normal;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;text-align:center;vertical-align:top">4</th><th style="font-family:Arial, Helvetica, sans-serif !important;;font-size:14px;font-weight:normal;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;text-align:center;vertical-align:top">6</th></tr><tr><td style="font-family:Arial, Helvetica, sans-serif !important;;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;text-align:center">7</td><td style="font-family:Arial, Helvetica, sans-serif !important;;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;text-align:center;vertical-align:top">6</td><td style="font-family:Arial, Helvetica, sans-serif !important;;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;text-align:center;vertical-align:top">5</td></tr><tr><td style="font-family:Arial, Helvetica, sans-serif !important;;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;text-align:center">8</td><td style="font-family:Arial, Helvetica, sans-serif !important;;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;text-align:center;vertical-align:top">7</td><td style="font-family:Arial, Helvetica, sans-serif !important;;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;text-align:center;vertical-align:top">6</td></tr><tr><td style="font-family:Arial, Helvetica, sans-serif !important;;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;text-align:center">3</td><td style="font-family:Arial, Helvetica, sans-serif !important;;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;text-align:center;vertical-align:top">1</td><td style="font-family:Arial, Helvetica, sans-serif !important;;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;text-align:center;vertical-align:top">5</td></tr></table>


변환 후
<table style="border-collapse:collapse;border-spacing:0">
<tr><th style="font-family:Arial, sans-serif;font-size:14px;font-weight:normal;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;text-align:center">2</th><th style="font-family:Arial, sans-serif;font-size:14px;font-weight:normal;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;text-align:center;vertical-align:top">2.67 </th><th style="font-family:Arial, sans-serif;font-size:14px;font-weight:normal;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;text-align:center;vertical-align:top">6</th></tr><tr><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;text-align:center">7.33</td><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;text-align:center;vertical-align:top">6</td><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;text-align:center;vertical-align:top">4</td></tr><tr><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;text-align:center">8</td><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;text-align:center;vertical-align:top">7.33</td><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;text-align:center;vertical-align:top">6</td></tr><tr><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;text-align:center">2</td><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;text-align:center;vertical-align:top">0.67</td><td style="font-family:Arial, sans-serif;font-size:14px;padding:10px 5px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;text-align:center;vertical-align:top">4</td></tr></table>

---

## 과제3
1. 영상 데이터의 히스토그램을 작성하고 히스토그램을 이용한 평활화를 적용하시오

---

## 설명4
영상에서 노이즈(잡음)은 영상의 정보를 훼손시키는 불필요한 정보를 의미합니다. 영상에서 노이즈가 발생하는 원인은 여러가지 요소가 있고 이로 인해 생기는 대표적인 노이즈의 종류는 다음과 같습니다.

## 노이즈

- 가우시안 노이즈

  전기, 열, 데이터 통신(전자 회로 노이즈)등 데이터의 수접 및 전송과정중에 발생하는 노이즈입니다. 자연스러운 노이즈의 분포가 대부분 종모양의 형태를 띄고 있습니다.
  
- 소금과 후추 노이즈

  아날로그 - 디지털 변환, 전송비트의 오류, 전압의 스파이크 등으로 인해 생기는 노이즈입니다. 이러한 노이즈가 발생한 픽셀은 검정, 흰색으로 나타나며 소금과 후추를 뿌린듯한 형태를 띄고 있습니다.

## 필터

이 외에도 샷노이즈, 양자화 노이즈 등 여러가지 노이즈의 형태가 존재합니다.
  
이러한 노이즈들은 영상을 처리하는 과정에서 필터를 사용하여 제거할 수 있습니다. 노이즈와 마찬가지로 대표적인 필터의 종류는 다음과 같습니다.

- 순차 필터

  인접한 화소를 작은값에서 큰 값으로 나열한 후 정렬된 화소로 부터 적절한 값을 선택하여 잡음을 제거합니다
    > 대표적으로 최소/최대 필터, 중간필터 등이 있습니다.

- 평균 필터

  인접한 화소간의 편균값을 선택하여 노이즈를 제거합니다.
  가우시언 노이즈과 균일 노이즈를 제거하는데 효과적이고 영상의 경계선을 흐리게 만들 수 있습니다.
    > 대표적으로 로우패스 필터가 있습니다.

- 적응적 필터

  이웃 화소 시스템 내의 밝기 값 특성 혹은 통계 값에 따라 필터의 적용방법을 변경합니다.
    > 대표적으로 최소평균오차 필터가 있습니다

- 가우시안 스무딩

  가우시안 분포를 적용한 것으로 정규분포, 확률분포로 생성된 잡음을 제거하기 위한 필터입니다.

  2차원 가우시안 함수와 유사한 커널로 convolution을 진행합니다
  
  <img src="http://homepages.inf.ed.ac.uk/rbf/HIPR2/figs/gausmask.gif" />
  
  ex) 5*5사이즈의 가우시안 커널
  
  >1차원 가우시안 분포는 다음과 같은 형태를 가집니다
  
  <img src="http://homepages.inf.ed.ac.uk/rbf/HIPR2/eqns/eqngaus1.gif" />
  
  <img src="http://homepages.inf.ed.ac.uk/rbf/HIPR2/figs/gauss1.gif" />
  
  >
  >2차원 순환대칭 가우시안은 다음과 같은 형태를 가집니다.
  
  <img src="http://homepages.inf.ed.ac.uk/rbf/HIPR2/eqns/eqngaus2.gif"/>
  
  <img src="http://homepages.inf.ed.ac.uk/rbf/HIPR2/figs/gauss2.gif" />
  

- Conservative Smoothing

  주위에 인접한 픽셀들의 밝기 값의 범위로 픽셀의 밝기를 고정시키는 방법입니다. 
  
  <img src="http://homepages.inf.ed.ac.uk/rbf/HIPR2/figs/csm3x3.gif"/>
  
  예를 들어 다음과 같은 이미지에서 인접한 픽셀의 밝기 범위는 115~127이므로 150픽셀의 밝기를 범위내의 최대치인 127픽셀로 고정합니다.
  이러한 필터를 통해 갑작스럽게 높거나 낮은 주파수를 가진 노이즈(소금가 후추 노이즈)를 제거하는데에 효율적이지만, 가우스 노이즈를 제거하는데에는 비효율적인 특성을 가지고 있습니다

### 과제4
1. 평균필터와 중간필터의 차이점을 설명하시오
2. 다음과 같은 영상 데이터를 Conservativ Smoothing를 진행할때 변화된 영상 데이터의 (2,2)픽셀의 값을 답하시오

<table><tr><th>121</th><th>132</th><th>129</th></tr><tr><td>136</td><td>119</td><td>148</td></tr><tr><td>131</td><td>142</td><td>133</td></tr></table>
