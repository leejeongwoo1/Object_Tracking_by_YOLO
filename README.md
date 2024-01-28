# Introduction
project name: Multi-Object Tracking by YOLO<br>
where: DAVI lab, Kyung Hee Univ.<br>
when: 2023.7 ~ 2023.8<br>

# How it works?
1. YOLO detects objects in one frame of video (ex. person, car). it can detects more than 50 category
2. Objects are indexed by its sort and order like "car1", "people0"
3. If Intersection of Union is high between two objects, it tracks object, otherwise it doesn't track object, just perceives as new object
4. Repeating this progess in every frame
# Flow chart
<div>multi-object detection</div>
<img src="https://github.com/leejeongwoo1/object_tracking_by_yolo/blob/main/detection_flowchart.jpg" width="500"/>

<div>multi-object tracking</div>
<img src="https://github.com/leejeongwoo1/object_tracking_by_yolo/blob/main/tracking_flowchart.jpg" width="500"/>

# How to use?
1. Git clone <a href="https://github.com/NizeLee/KhuCv_mdi">KhuCV repo</a>
2. Build KhuCV app refer to README at KhuCV. you can also refer this <a href="https://curvy-bathtub-cb1.notion.site/1-wxWidget-opencv-703c6fc832e144b499652e4d23cdff05">notion</a>
3. Replace .cpp and .h files in KhuCV with my files
4. Add <a href="https://pjreddie.com/darknet/yolo/">yolov3-tiny.weights, yolov3-tiny.cfg</a> to execution folder (ex. RUN64)
5. Input video to app, then you can see it works well^^
   
# Demo
<img src="https://github.com/leejeongwoo1/object_tracking_by_yolo/blob/main/demo_picture.png" width="400"/>
<a href="https://file.notion.so/f/f/44cea39e-602e-4505-bfbd-ca610fd8c469/dd093981-4872-4689-874a-ccf7130aa186/KhuCV_App_-_KhuCv_Image_2023-08-11_02-24-17.mp4?id=8a39771c-5400-425a-bf6f-9d1ca98bcd1e&table=block&spaceId=44cea39e-602e-4505-bfbd-ca610fd8c469&expirationTimestamp=1706536800000&signature=_7aV21-qSou1tYfptXHwCc8tnCAuYuyxZbgvtWU_kyo&downloadName=KhuCV+App+-+%5BKhuCv+Image%5D+2023-08-11+02-24-17.mp4">Video</a>

# Reference
<a href="https://curvy-bathtub-cb1.notion.site/6-YOLOX-1551f188f0944f1482d973a17bad96dd">multi-object tracking using yolo</a>
<p>If you want learn more, read explanation ppt!^^</p>

