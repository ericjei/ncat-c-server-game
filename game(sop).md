SOP
視窗a
進入資料夾
cd ~/ctf_project
清除執行檔
make clean
編譯
make 
監聽
(現已取代)start_game
*ttyd -W -p 8000 ./ctf_server
socat TCP-LISTEN:8000,reuseaddr,fork EXEC:"./ctf_server"
----------------------------------------------------------------------------------------
b視窗
外接
ssh -p 443 -R0:localhost:8000 tcp@a.pinggy.io
ssh -p 443 -R0:localhost:8000 a.pinggy.io
*ngrok http 8000
複製網址
--------------------------------------------------------------------------------------------
