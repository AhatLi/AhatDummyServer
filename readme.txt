프로그램 설명
더미서버의 역할을 하는 프로그램으로 추가파일을 넣어줘서 여러 API에 대한 더미서버역할을 여러 포트를 열어 할 수 있다.
현재는 http에 대한 요청만 전달이 가능하다.


사용방법

1.실행파일과 같은 경로에 AhatDummyServerAPI(실행파일명 + API)I 폴더를 생성한다

2.해당 폴더 안에 API 주소와 같은 경로로 파일을 만들고 파일의 내용은 response로 날아갈 데이터를 넣는다.
ex : 
http://127.0.0.1:8888/API/APC/API/GetFileData 
라는 API에 대한 응답을 만들고 싶으면
AhatDummyServerAPI/APC/API 경로에 GetFileData 파일을 만든다.

3. AhatDummyServer 를 실행한다 뒤에 포트주소를 넣어주면 되고 포트는 여러개를 사용 가능하다.
ex : [root@localhost dummyserver]# ./AhatDummyServer 8887 8888 8889


포트별로 스레드가 생성되며 각 스레드의 동작은 포트주소만 다를 뿐 모두 같다.

아래는 API 추가파일에 들어갈 데이터 샘플이며 
아래청럼 헤더를 넣어주지 않으면 http response로써 동작하지 않는다.

HTTP/1.1 200 OK
Accept: *
Connection: close
Content-Type: application/json
Content-Length:18

{"resultCode":200}