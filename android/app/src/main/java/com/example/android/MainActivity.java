package com.example.android;

import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

import androidx.activity.EdgeToEdge;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.graphics.Insets;
import androidx.core.view.ViewCompat;
import androidx.core.view.WindowInsetsCompat;

import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;

public class MainActivity extends AppCompatActivity {
    private String STAip, STAport;
    private EditText ipText, portText;
    private TextView ipView;
    private Button sendButton, connectButton, sendButton2, sendButton3;
    
    private DatagramSocket udpSocket;
    private int port;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        ipText = findViewById(R.id.ipText);
        portText = findViewById(R.id.portText);
        ipView = findViewById(R.id.ipView);
        sendButton = findViewById(R.id.sendButton);
        sendButton2 = findViewById(R.id.sendButton2);
        sendButton3 = findViewById(R.id.sendButton3);
        connectButton = findViewById(R.id.connectButton);
        EdgeToEdge.enable(this);
        ViewCompat.setOnApplyWindowInsetsListener(findViewById(R.id.main), (v, insets) -> {
            Insets systemBars = insets.getInsets(WindowInsetsCompat.Type.systemBars());
            v.setPadding(systemBars.left, systemBars.top, systemBars.right, systemBars.bottom);
            return insets;
        });

        connectButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                String ip = ipText.getText().toString();
                port = Integer.parseInt(portText.getText().toString());
                new Thread(() -> {
                    try {
                        udpSocket = new DatagramSocket();
                        runOnUiThread(() -> {
                            ipView.setText("Connected to " + ip + ":" + port);
                        });
                    } catch (Exception e) {
                        e.printStackTrace();
                        runOnUiThread(() -> {
                            ipView.setText("Connection failed");
                        });
                    }
                }).start();
            }
        });

        sendButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                sendCommand("pwr=1prst=0RST=0");
            }
        });

        sendButton2.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                sendCommand("pwr=2prst=0RST=0");
            }
        });

        sendButton3.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                sendCommand("pwr=0prst=1RST=0");
            }
        });
    }

    private void sendCommand(String command) {
        if (udpSocket != null) {
            new Thread(() -> {
                try {
                    byte[] buffer = command.getBytes();
                    DatagramPacket packet = new DatagramPacket(buffer, buffer.length, InetAddress.getByName(ipText.getText().toString()), port);
                    udpSocket.send(packet);
                    runOnUiThread(() -> {
                        ipView.setText("Message sent: " + command);
                    });
                } catch (Exception e) {
                    e.printStackTrace();
                    runOnUiThread(() -> {
                        ipView.setText("Failed to send message");
                    });
                }
            }).start();
        } else {
            runOnUiThread(() -> {
                ipView.setText("Not connected to server");
            });
        }
    }
}