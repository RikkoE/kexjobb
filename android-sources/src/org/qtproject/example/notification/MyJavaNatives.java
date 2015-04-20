package org.qtproject.example.notification;

public class MyJavaNatives {

    public static native void sendFibonaciResult(int n);
    public static native void sendScanResult(String[] array);

    public static String print() {
        sendFibonaciResult(10);
        return "Hola mundos!!!";
    }
}

