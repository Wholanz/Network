/**
 * Created by LK on 2015/6/15.
 */
import sun.java2d.loops.GeneralRenderer;
import sun.misc.BASE64Decoder;

import java.io.*;
import java.net.InetAddress;
import java.net.ServerSocket;
import java.net.Socket;
import java.security.KeyFactory;
import java.security.PrivateKey;
import java.security.PublicKey;
import java.security.spec.X509EncodedKeySpec;


public class myHttpServer1 extends Thread {

    private byte[] content;
    private byte[] header;
    private byte[] postForNewPage;
    private byte[] content2;
    private int port=2222;

    private String password = "password to decrypt the data from server";

    final String id = "Right Server";


    private PublicKey pubkey ;
    private PrivateKey prikey ;


    private myHttpServer1(String data, String encoding, String MIMEType, int port) throws UnsupportedEncodingException {
        this(data.getBytes(encoding), encoding, MIMEType, port);

        GenerateKeyPair gen = new GenerateKeyPair();
        pubkey = gen.getPubKey();
        prikey = gen.getPriKey();
    }

    public myHttpServer1(byte[] data, String encoding, String MIMEType, int port)throws UnsupportedEncodingException {
        this.content=data;
        this.port=port;



        String header="HTTP/1.0 200 OK\r\n"+
                "Server: OneFile 1.0\r\n"+
                "Content-length: "+this.content.length+"\r\n"+
                "Content-type: "+MIMEType+"\r\n\r\n";
        this.header=header.getBytes("ASCII");
        String postToAnotherServer = "Another POST /server2.html HTTP/1.1\r\n"+
                "Accept: */*\r\n"+
                "Accept-Language: zh-cn\r\n"+
                "host: localhost\r\n\r\n";

        this.postForNewPage = postToAnotherServer.getBytes("ASCII");


        GenerateKeyPair gen = new GenerateKeyPair();
        pubkey = gen.getPubKey();
        prikey = gen.getPriKey();


    }


    public void run() {
        try {
            ServerSocket server=new ServerSocket(this.port);
            System.out.println("Accepting connections on port "+server.getLocalPort());
            System.out.println("Data to be sent:");
            System.out.write(this.content);

            while (true) {
                Socket connection=null;
                try {
                    connection=server.accept();
                    OutputStream out=new BufferedOutputStream(connection.getOutputStream());
                    InputStream in=new BufferedInputStream(connection.getInputStream());

                    StringBuffer request=new StringBuffer();
                    while (true) {
                        int c=in.read();
                        if (c=='\r'||c=='\n'||c==-1) {
                            break;
                        }
                        request.append((char)c);

                    }
                    System.out.println("Request " + request);
                        /*
                        //如果检测到是HTTP/1.0及以后的协议，按照规范，需要发送一个MIME首部
                        if (request.toString().indexOf("HTTP/")!=-1) {
                            out.write(this.header);
                        }

                        out.write(this.content);
                        out.flush();
                        */
                    if (request.toString().indexOf("GET") >= 0){
                        if (request.toString().indexOf("favicon") >= 0)
                            continue;
                        if (request.toString().indexOf("HTTP/") >= 0)
                            out.write(header);
                        System.out.println("Get process the message");
                        out.write(content);
                        out.flush();
                    }
                    else
                    if (request.toString().indexOf("POST") >= 0){


                        if (request.toString().indexOf("jumpto1") >= 0 || request.toString().indexOf("server1.html") >= 0){

                            if (request.toString().indexOf("Another") >= 0) {

                                System.out.println("local port :" + connection.getLocalPort());
                                System.out.println("port :" + connection.getPort());

                                out.write(GenerateKeyPair.bytesToHexStr(pubkey.getEncoded()).getBytes("ASCII"));
                                out.flush();

                                System.out.println(pubkey.toString().getBytes("ASCII"));

                                out.write("complete".getBytes());
                                out.flush();

/*****************************************************************************************************/
                                SignatureData sig = new SignatureData();

                                byte[] signed = sig.run(prikey, new String(id));
                                out.write(GenerateKeyPair.bytesToHexStr(signed).getBytes("ASCII"));
                                out.flush();
                                System.out.println("signed :" + GenerateKeyPair.bytesToHexStr(signed));
                                out.write("complete".getBytes());
                                out.flush();

/*****************************************************************************************************/
                                byte[] encryptContent = DES.encrypt(content, password);

                                out.write(GenerateKeyPair.bytesToHexStr(encryptContent).getBytes("ASCII"));
                                out.flush();

                                out.write("complete".getBytes("ASCII"));
                                out.flush();
                            }else{
                                out.write(content);
                                out.flush();
                            }
                            System.out.println("haha");
                        }
                        else
                        {
                            System.out.println("client wants to jump!");
                            String anotherIp = "127.0.0.1";
                            int anotherPort = 81;
                            Socket anotherServer = new Socket(anotherIp, anotherPort);
                            OutputStream anotherOut=new BufferedOutputStream(anotherServer.getOutputStream());
                            InputStream anotherIn=new BufferedInputStream(anotherServer.getInputStream());

                            int c;
                            StringBuffer response = new StringBuffer();



                            anotherOut.write(postForNewPage);
                            anotherOut.flush();

//                            anotherOut.write("Another".getBytes("ASCII"));
//                            anotherOut.flush();

                            response = new StringBuffer();
                            System.out.println("From server2 socket:");



                            while (true){
                                c = anotherIn.read();
                                System.out.print((char)c);
                                response.append((char)c);
                                if (response.toString().indexOf("complete") >= 0) {
                                    response.delete(response.toString().indexOf("complete"), response.length());
                                    System.out.println("response length: " + response.length());
                                    break;
                                }
                            }


                            PublicKey ServerPubkey = null;



                            X509EncodedKeySpec bobPubKeySpec = new X509EncodedKeySpec(GenerateKeyPair.hexStrToBytes(response.toString()));
                            try {
                                KeyFactory keyFactory = KeyFactory.getInstance("RSA");
                                ServerPubkey = keyFactory.generatePublic(bobPubKeySpec);
                            }catch(Exception e){
                                e.printStackTrace();
                            }

                            System.out.println("Key get complete");
//
                            response = new StringBuffer();

                            while (true){
                                c = anotherIn.read();
                                System.out.print((char)c);
                                response.append((char)c);
                                if (response.toString().indexOf("complete") >= 0){
                                    response.delete(response.toString().indexOf("complete"), response.length());
                                      break;
                                }
                            }

                            VerifySignature verifySignature = new VerifySignature();

                            byte[] signed = GenerateKeyPair.hexStrToBytes(response.toString());

                            System.out.println("signed value:" + response.toString());

                            response = new StringBuffer();

                            while (true){
                                c = anotherIn.read();
                                System.out.print((char)c);
                                response.append((char)c);

                                if (response.toString().indexOf("complete") >= 0){
                                    response.delete(response.toString().indexOf("complete"), response.length());
                                    break;
                                }
                            }

                            byte[] byteContent2 = GenerateKeyPair.hexStrToBytes(response.toString());

                            System.out.println("Encrypted message :" + response.toString());

                            byte[] decryptContent2 = null;
                            try {
                                decryptContent2 = DES.decrypt(byteContent2, password);
                            }catch (Exception e){

                                e.printStackTrace();
                            }

                            String ErrorMsg = "<html><h>Signature Error</h></html>";
                            if(ServerPubkey == null){

                                System.out.println("Missing Server Public Key!");
                                return;
                            }
                            if (verifySignature.run(ServerPubkey,signed,id)){
                                try {
                                    out.write(decryptContent2);
                                }catch (Exception e){
                                    e.printStackTrace();
                                }
                            }
                            else{
                                out.write(ErrorMsg.getBytes("ASCII"));
                            }
                            out.flush();
                            System.out.println();
                            System.out.println("From server2: "+content2);
                        }

                    }
                    else{
                        System.out.println("deal with remains");
                        try{
                            //Thread.sleep(1000);
                        }catch(Exception e){

                        }
                    }

                } catch (IOException e) {
                    // TODO: handle exception
                }finally{
                    if (connection!=null) {
                        connection.close();
                    }
                }
            }

        } catch (IOException e) {
            System.err.println("Could not start server. Port Occupied");
        }
    }
    public static void main(String[] args) {
        try {
            String contentType="text/html";
            /*
            if (args[0].endsWith(".html")||args[0].endsWith(".htm")) {
                contentType="text/html";
            }
            */

            //InputStream in=new FileInputStream(args[0]);
            InputStream in=new FileInputStream("server1.html");
            ByteArrayOutputStream out=new ByteArrayOutputStream();
            int b;
            while ((b=in.read())!=-1) {
                out.write(b);
            }
            in.close();
            byte[] data=out.toByteArray();
            out.close();
            //设置监听端口
            int port=2222;
            /*
            try {
                port=Integer.parseInt(args[1]);
                if (port<1||port>65535) {
                    port=80;
                }
            } catch (Exception e) {
                port=80;
            }
            */
            String encoding="ASCII";
            /*
            if (args.length>2) {
                encoding=args[2];
            }
            */

            Thread t=new myHttpServer1(data, encoding, contentType, port);
            t.start();

        } catch (ArrayIndexOutOfBoundsException e) {
            System.out.println("Usage:java myHttpServer filename port encoding");
        }catch (Exception e) {
            System.err.println(e);// TODO: handle exception
        }
    }
}

