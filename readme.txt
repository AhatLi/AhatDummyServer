���α׷� ����
���̼����� ������ �ϴ� ���α׷����� �߰������� �־��༭ ���� API�� ���� ���̼��������� ���� ��Ʈ�� ���� �� �� �ִ�.
����� http�� ���� ��û�� ������ �����ϴ�.


�����

1.�������ϰ� ���� ��ο� AhatDummyServerAPI(�������ϸ� + API)I ������ �����Ѵ�

2.�ش� ���� �ȿ� API �ּҿ� ���� ��η� ������ ����� ������ ������ response�� ���ư� �����͸� �ִ´�.
ex : 
http://127.0.0.1:8888/API/APC/API/GetFileData 
��� API�� ���� ������ ����� ������
AhatDummyServerAPI/APC/API ��ο� GetFileData ������ �����.

3. AhatDummyServer �� �����Ѵ� �ڿ� ��Ʈ�ּҸ� �־��ָ� �ǰ� ��Ʈ�� �������� ��� �����ϴ�.
ex : [root@localhost dummyserver]# ./AhatDummyServer 8887 8888 8889


��Ʈ���� �����尡 �����Ǹ� �� �������� ������ ��Ʈ�ּҸ� �ٸ� �� ��� ����.

�Ʒ��� API �߰����Ͽ� �� ������ �����̸� 
�Ʒ�û�� ����� �־����� ������ http response�ν� �������� �ʴ´�.

HTTP/1.1 200 OK
Accept: *
Connection: close
Content-Type: application/json
Content-Length:18

{"resultCode":200}