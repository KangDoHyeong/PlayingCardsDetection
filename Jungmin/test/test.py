import numpy as np
import cv2
import os
# import time

def preprocess_perspective(file_name):
    ## contours를 찾는 작업
    tresh_min = 100
    tresh_max = 255

    image = cv2.imread(file_name)
    im_bw = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    thresh, im_bw = cv2.threshold(im_bw, tresh_min, tresh_max, 0)
    contours, hierarchy = cv2.findContours(im_bw, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
    
    # 모서리를 찾는 작업
    rects = [cv2.minAreaRect(each) for each in contours]
    rects = [(a,b,c) for a, b, c  in rects if ((b[0]*b[1]>9000))] # 값 9000은 추후 수정 필요
    if len(rects)>1:
        return "too many boundary" # card boundary(사각형)를 여러개 찾았으니 error 표시
    
    box = cv2.boxPoints(rects[0])
    box = box.astype('int')
        
    temp_rect = np.array(box, dtype = "float32")
    temp_rect = temp_rect[np.argsort(temp_rect[:, 1])]
    if temp_rect[0][0]>temp_rect[1][0]:
        temp = temp_rect[0].copy()
        temp_rect[0] = temp_rect[1]
        temp_rect[1] = temp
    if temp_rect[2][0]<temp_rect[3][0]:
        temp = temp_rect[2].copy()
        temp_rect[2] = temp_rect[3]
        temp_rect[3] = temp
    # wrapPerspective 하는 작업
    maxWidth = 100
    maxHeight = 120
    dst = np.array([[0,0],[maxWidth-1,0],[maxWidth-1,maxHeight-1],[0, maxHeight-1]], np.float32)
    M = cv2.getPerspectiveTransform(temp_rect,dst)
    warp = cv2.warpPerspective(image, M, (maxWidth, maxHeight)) # warp에 이미지 담김
    warp = cv2.cvtColor(warp,cv2.COLOR_BGR2GRAY)
    
    # wrap 이미지랑 꼭지점 담긴 행렬 돌려줌
    return warp #, temp_rect

def preprocess_perspective_simple(file_name):
    ## contours를 찾는 작업
    tresh_min = 100
    tresh_max = 255

    image = cv2.imread(file_name)
    im_bw = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    thresh, im_bw = cv2.threshold(im_bw, tresh_min, tresh_max, 0)
    contours, hierarchy = cv2.findContours(im_bw, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
    
    # 모서리를 찾는 작업
    rects = [cv2.minAreaRect(each) for each in contours]
    rects = [(a,b,c) for a, b, c  in rects if ((b[0]*b[1]>9000))] # 값 9000은 추후 수정 필요
    if len(rects)>1:
        return "too many cards" # 사각형을 여러개 찾았으니 error 표시
    
    box = cv2.boxPoints(rect)
    
    box = box[np.argsort(box[:, 1])]
    if box[0][0]>box[1][0]:
        temp = box[0].copy()
        box[0] = box[1]
        box[1] = temp
    if box[2][0]<box[3][0]:
        temp = box[2].copy()
        box[2] = box[3]
        box[3] = temp
    # wrapPerspective 하는 작업
    maxWidth = 100
    maxHeight = 120
    dst = np.array([[0,0],[maxWidth-1,0],[maxWidth-1,maxHeight-1],[0, maxHeight-1]], np.float32)
    M = cv2.getPerspectiveTransform(box,dst)
    warp = cv2.warpPerspective(image, M, (maxWidth, maxHeight)) # warp에 이미지 담김
    warp = cv2.cvtColor(warp,cv2.COLOR_BGR2GRAY)
    
    # wrap 이미지랑 꼭지점 담긴 행렬 돌려줌
    return warp #, temp_rect

def origin_card_info():
    cardlist=[]
    origin_card = os.listdir('./origin_card')

    for i in origin_card:
        cardlist.append(i[:-4])
    
    

    cards = []
    for i in origin_card:
    #     print(i)
        cards.append(cv2.imread('./origin_card/'+i, cv2.IMREAD_GRAYSCALE))
    cards = np.array(cards)

    return cardlist, cards

def shape_num(warp_image):
    cardlist, cards = origin_card_info()

    threshold = 2000000

    min_img = threshold + 50000
    min_num = 0

    for i in range(len(cardlist)):
        diff_img = cv2.absdiff(cards[i],warp_image).sum()
        if diff_img<min_img:
            min_img=diff_img
            min_num=i
    if min_img>=threshold:
        return "can't classify card"
    
    return cardlist[min_num]


file_name = '../../big_au/sample2/C10_0_5434.jpg'
warp_img = preprocess_perspective(file_name)
card = shape_num(warp_img)
print(card)

            
        