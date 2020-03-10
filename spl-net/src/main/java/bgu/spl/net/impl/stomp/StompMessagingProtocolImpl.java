package bgu.spl.net.impl.stomp;

import bgu.spl.net.api.StompMessagingProtocol;
import bgu.spl.net.srv.Connections;

import java.util.Map;
import java.util.concurrent.atomic.AtomicInteger;

public class StompMessagingProtocolImpl<T> implements StompMessagingProtocol {

    //Fields
    private boolean shouldTerminate = false;
    private Connections<String> connections;
    private AtomicInteger messageID;
    private int connectionID;

    //Methods
    @Override
    public void start(int connectionId, Connections<String> connections) {
        this.connections = connections;
        this.connectionID = connectionId;
        messageID = new AtomicInteger(1);
    }

    @Override
    public void process(String message) {

        StompMessage stompMessage = new StompMessage(message);
        String command = stompMessage.getHeadline();
        String receiptID = stompMessage.getHeader("receipt");
        String destination = stompMessage.getHeader("destination");

        StompMessage toSend = new StompMessage();

        try {
            switch (command) {
                case "SEND":
                    String body = stompMessage.getBody();
                    String messageIDstr = Integer.toString(messageID.getAndIncrement());
                    toSend = createMessage(body, messageIDstr, destination);
                    connections.send(destination, toSend.toString());
                    break;

                case "CONNECT":
                    String version = stompMessage.getHeader("accept-version");
                    String userName = stompMessage.getHeader("login");
                    String password = stompMessage.getHeader("passcode");


                    connections.login(userName, password, connectionID);
                    toSend.setHeadline("CONNECTED");
                    toSend.setHeader("version", version);

                    connections.send(connectionID, toSend.toString());
                    break;

                case "SUBSCRIBE":
                    connections.joinTopic(connectionID, destination, getSubID(stompMessage));
                    toSend.setHeadline("RECEIPT");
                    toSend.setHeader("receipt-id", receiptID);
                    connections.send(connectionID, toSend.toString());
                    break;


                case "DISCONNECT":
                    shouldTerminate = true;
                    connections.disconnect(connectionID);
                    toSend = createReceipt(receiptID);
                    connections.send(connectionID, toSend.toString());
                    break;

                case "UNSUBSCRIBE":
                    connections.exitTopic(connectionID, getSubID(stompMessage));
                    toSend = createReceipt(receiptID);
                    connections.send(connectionID, toSend.toString());
                    break;
            }

        } catch (Exception e) {
            StompMessage errorMsg = createError(e.getMessage(), receiptID);
            connections.send(connectionID, errorMsg.toString());
            shouldTerminate = true;
            connections.disconnect(connectionID);
        }
    }

    @Override
    public boolean shouldTerminate() {
        return shouldTerminate;
    }


    //Help Methods
    public StompMessage createError(String error, String receiptID){
        StompMessage errorMsg = new StompMessage("");
        errorMsg.setHeadline("ERROR");
        errorMsg.setHeader("receipt-id", receiptID);
        errorMsg.setHeader("message", error);

        return errorMsg;
    }

    public StompMessage createMessage(String body, String messageIDstr, String destination){
        StompMessage Message = new StompMessage();
        Message.setHeadline("MESSAGE");
        Message.setHeader("Message-id", messageIDstr);
        Message.setHeader("destination", destination);
        Message.setBody(body);


        return Message;
    }

    public StompMessage createReceipt(String receiptID){
        StompMessage receipt = new StompMessage("");
        receipt.setHeadline("RECEIPT");
        receipt.setHeader("receipt-id",receiptID);

        return receipt;
    }

    public int getSubID(StompMessage stompMessage){
        String subID = stompMessage.getHeader("id");
        Integer subIDint = Integer.valueOf(subID);
        return subIDint;
    }
}

