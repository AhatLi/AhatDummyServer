프로그램 설명
더미 서버의 역할을 하는 프로그램으로 추가파일을 넣어줘서 여러 API에 대한 더미 서버 역할을, 여러개의 포트를 열어서 동작한다.
현재는 http에 대한 요청만 전달이 가능하다.


사용방법

1. 실행파일과 같은 경로에 AhatDummyServerAPI(실행파일명 + API) 폴더를 생성한다

2. 해당 폴더 안에 API 주소와 같은 경로로 파일을 만들고 파일의 내용은 response로 날아갈 데이터를 넣는다.
ex : 
http://127.0.0.1:8888/API/APC/API/GetFileData 
라는 API에 대한 응답을 만들고 싶으면
AhatDummyServerAPI/APC/API 경로에 GetFileData 파일을 만든다.

3. AhatDummyServer를 실행한다 뒤에 포트 주소를 넣어주면 되고 포트는 여러 개를 사용 가능하다.
ex : [root@localhost dummyserver]# ./AhatDummyServer 8887 8888 8889


현재로써는 포트별로 스레드가 생성되며 각 스레드의 동작은 포트주소만 다를 뿐 모두 같다.

아래는 API 추가파일에 들어갈 데이터 샘플이며 
아래청럼 헤더를 넣어주지 않으면 http response로써 동작하지 않는다.

HTTP/1.1 200 OK
Accept: *
Connection: close
Content-Type: application/json
Content-Length:18

{"resultCode":200}




스크립트 형식 예제)
#script
#if port 8000
	#header-code=200
	#header-content-type=application/json
	#body-type=python3.8
	#body-file=test.py
	#body-function=func1
	#body-function-param_num=2
	#body-function-param1=${param=param1}
	#body-function-param2=${requestBody}
#end
#if port 7000
	#header-code=200
	#body-type=python3.8
	#body-file=test.py
	#body-function=func1
	#body-function-param_num=2
	#body-function-param1="string"
	#body-function-param2=13
#end
#if port 6000
	#header-code=404
	#body-type=raw
file not found
#end
#if port all
	#header-content-type=text
	#body-type=raw
텍스트 시작부터 끝까지 body로 친다
설마 #} 라는 텍스트가 필요한 body가 있지는 않겠지

있을수도 있겠지만 나중에 \#} 이런식으로 해놔야겠다.
#end