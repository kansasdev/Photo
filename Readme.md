This application is a frontend to OFIQ library which allows you to calculate face quality based on 0-100 score.Working with camera directly requires usage of PhotoUWP package created by kansasdev. After taking image from camera it is required to crop it manually and selecting save from save options. App is not sending data anywhere. PhotoUWP saves image taken to pictures library (face.bmp) then WpfCamera loads image and performs quality checks using OFIQ library.
                                      OFIQ library is created by BSI and it is public. OFIQ library relays on some face detect train DNN models like ssd facedetect and ADNet. Application also uses WpfAboutView control to display some basic informations. For best end user experience, as camera setup for classic Windows is quite limited, you can install PhotoUWP application which allows to take live image face using your camera connected to PC. This application doesn't store any data and doesn't send it anywhere.
Since github has some file size limits, to work it correctly you need to download and place file train.pkl from https://drive.google.com/drive/folders/1jkJ0VruvJJNtsBruqdUMxQwpWtA9dxkk
into prereq\models\adnet. These files came from OFIQ project.
Additionally application is uing OpenCV library and control WpfAboutView (Copyright (c) 2017 Daniel Chalmers)