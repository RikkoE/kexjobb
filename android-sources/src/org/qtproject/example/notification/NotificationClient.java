/****************************************************************************
**
** Copyright (C) 2014 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtAndroidExtras module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL21$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia. For licensing terms and
** conditions see http://qt.digia.com/licensing. For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights. These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** $QT_END_LICENSE$
**
****************************************************************************/

package org.qtproject.example.notification;

import android.app.Notification;
import android.app.NotificationManager;
import android.content.Context;
import android.bluetooth.BluetoothAdapter;
import android.content.Intent;
import android.bluetooth.BluetoothDevice;
import android.content.BroadcastReceiver;
import android.content.IntentFilter;
import android.widget.ArrayAdapter;
import android.widget.Toast;


public class NotificationClient extends org.qtproject.qt5.android.bindings.QtActivity
{
    private static NotificationManager m_notificationManager;
    private static Notification.Builder m_builder;
    private static NotificationClient m_instance;

    private static native void callNativeFunction(int x);

    public NotificationClient()
    {
        m_instance = this;
    }

//    public static void notify(String s)
//    {
//        if (m_notificationManager == null) {
//            m_notificationManager = (NotificationManager)m_instance.getSystemService(Context.NOTIFICATION_SERVICE);
//            m_builder = new Notification.Builder(m_instance);
//            m_builder.setSmallIcon(R.drawable.icon);
//            m_builder.setContentTitle("A message from Qt!");
//        }

//        m_builder.setContentText(s);
//        m_notificationManager.notify(1, m_builder.build());
//    }

    private BluetoothAdapter bluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
    private static String[] devicesFound = new String[20];
    private static String[] sendList;

    static int deviceNumber = 0;
    static int nrOfDevices = 0;



    private static boolean searching = true;

    BroadcastReceiver mReceiver = new BroadcastReceiver() {
                public void onReceive(Context context, Intent intent) {
                    String action = intent.getAction();

                    if (BluetoothAdapter.ACTION_DISCOVERY_STARTED.equals(action)) {
                        searching = true;
                        //discovery starts, we can show progress dialog or perform other tasks
                    } else if (BluetoothAdapter.ACTION_DISCOVERY_FINISHED.equals(action)) {
                        searching = false;
                        //discovery finishes, dismiss progress dialog
                    } else if (BluetoothDevice.ACTION_FOUND.equals(action)) {
                        //bluetooth device found
                        BluetoothDevice device = (BluetoothDevice) intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
                        System.out.println("Found a device");
                        if(device.getName() == "null") {
                            devicesFound[deviceNumber] = device.getAddress();
                        } else {
                            devicesFound[deviceNumber] = device.getName();
                        }

                        deviceNumber++;
                        nrOfDevices++;
//                        devicesFound[0] = device.getName();
                    }
                }
            };

    public static void scan() {
        m_instance.bluetoothAdapter = BluetoothAdapter.getDefaultAdapter();

//            btArrayAdapter.clear();
            IntentFilter filter = new IntentFilter();
            m_instance.bluetoothAdapter.startDiscovery();

            filter.addAction(BluetoothDevice.ACTION_FOUND);
            filter.addAction(BluetoothAdapter.ACTION_DISCOVERY_STARTED);
            filter.addAction(BluetoothAdapter.ACTION_DISCOVERY_FINISHED);

            m_instance.registerReceiver(m_instance.mReceiver, filter);

    }

    public static String[] scanReturn() {
        m_instance.bluetoothAdapter = BluetoothAdapter.getDefaultAdapter();

            IntentFilter filter = new IntentFilter();
            m_instance.bluetoothAdapter.startDiscovery();

            filter.addAction(BluetoothDevice.ACTION_FOUND);
            filter.addAction(BluetoothAdapter.ACTION_DISCOVERY_STARTED);
            filter.addAction(BluetoothAdapter.ACTION_DISCOVERY_FINISHED);

//            devicesFound[0] = "Hello!";

            m_instance.registerReceiver(m_instance.mReceiver, filter);



            while(searching);
            searching = true;

            if(nrOfDevices > 0){
                sendList = new String[nrOfDevices];
                System.out.println("nrOfDevices: " + nrOfDevices);
                for(int i=0;i<nrOfDevices;i++) {
                    System.out.println("Device name: "+devicesFound[i]);
                    sendList[i] = devicesFound[i];
                }

                System.out.println("Device 0: " + sendList[0]);
            } else {
                sendList = new String[1];
                sendList[0] = "no devices";
            }
            nrOfDevices = 0;
            deviceNumber = 0;
            return sendList;
//            return devicesFound;
    }

    public static void btON() {
        System.out.println("asdf");
        System.out.println(m_instance.bluetoothAdapter.isEnabled());
        if (!m_instance.bluetoothAdapter.isEnabled()) {
            System.out.println("bluetooth is turned off");
//             m_instance.startActivityForResult(new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE), 0);
            m_instance.bluetoothAdapter.enable();
         }
//         else {
//             System.out.println("turning off bluetooth!!");
//             m_instance.bluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
//             m_instance.bluetoothAdapter.disable();
//         }

        callNativeFunction(3);


    }
    public static void btOFF() {
        System.out.println("Bluetooth off");
        m_instance.bluetoothAdapter.disable();
    }


}
