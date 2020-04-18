import java.io.*;
import java.net.*;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;

public class client {
    public static int recibeRespuesta(DatagramSocket s) {
        try {
            System.out.println("[ WAIT ] - Esperando respuesta...");
            byte[] buffer = new byte[4];
            DatagramPacket reply = new DatagramPacket(buffer, buffer.length);
            s.receive(reply);
            byte[] data = reply.getData();
            ByteBuffer bufResponse = ByteBuffer.wrap(data).order(ByteOrder.LITTLE_ENDIAN);
            int r = bufResponse.getInt();
            return r;
        } catch (IOException e) {
            return 0;
        }
    }
    public static void main(String args[]){
        int port = 5000;
        double PI = 3.14159265;
        int terminos = 1;
        try{
            DatagramSocket socketUDP = new DatagramSocket();
            InetAddress host = InetAddress.getByName("192.168.0.7");
            // Alojamos Bytes para la fn Original 
            ByteBuffer fnX = ByteBuffer.allocate(5032).order(ByteOrder.LITTLE_ENDIAN);
            ByteBuffer fnY = ByteBuffer.allocate(5032).order(ByteOrder.LITTLE_ENDIAN);
            // Alojamos Bytes para la fn de fourier
            ByteBuffer bufX = ByteBuffer.allocate(50272).order(ByteOrder.LITTLE_ENDIAN);
            ByteBuffer bufY = ByteBuffer.allocate(50272).order(ByteOrder.LITTLE_ENDIAN);
            System.out.println("[ OK ] - Generando f(x) = x");
            double x, y;
            for(x=-PI ; x<=PI ; x+=0.01){
                y = -x;
                fnX.putDouble(x);
                fnY.putDouble(y);
            } 
            DatagramPacket dp = new DatagramPacket(fnX.array(), fnX.limit(), host , port);
            System.out.println("[ SEND ] - Enviando f(x) = x parte X");
            socketUDP.send(dp);

            while(true){
                //Estaremos recibiendo y enviando ciclicamente a partir de aqui
                int r = recibeRespuesta(socketUDP);
                switch(r){
                    case 1:
                        //Nos pide fn Parte Y
                        dp = new DatagramPacket(fnY.array(), fnY.limit(), host , port);
                        System.out.println("[ SEND ] - Enviando f(x) = x parte Y");
                        socketUDP.send(dp);
                    break;
                    case 2:
                        //Pide calcular fourier y enviar parte X 
                        System.out.println("[ OK ] - Generando Fourier de f(x) = x, con termino: "+terminos);
                        bufX.clear();
                        bufY.clear();
                        for(x=-PI ; x<=PI ; x+=0.001){
                            y = 0;
                            for(int n = 1; n <= terminos; n++){
                               int signo = (int) Math.pow(-1, (n + 1));
                               y += signo * ( Math.sin(n*-x) / n );
                            }
                            y = y * 2;
                            bufX.putDouble(x);
                            bufY.putDouble(y);
                        }
                        dp = new DatagramPacket(bufX.array(), bufX.limit(), host , port);
                        System.out.println("[ SEND ] - Enviando Fourier, parte X");
                        socketUDP.send(dp);
                    break;
                    case 3:
                        //Pide calcular fourier y enviar parte Y
                        dp = new DatagramPacket(bufY.array(), bufY.limit(), host , port);
                        System.out.println("[ SEND ] - Enviando Fourier, parte Y");
                        socketUDP.send(dp);
                        terminos++;
                        /*
                        if(terminos>4){
                            terminos = 1;
                        }
                        */
                    break;
                    default:
                    break;
                }
            }
            
        }catch(IOException e){System.err.println("IOException " + e);}
    }
}