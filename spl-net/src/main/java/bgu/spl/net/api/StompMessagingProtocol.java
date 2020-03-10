package bgu.spl.net.api;

import bgu.spl.net.impl.stomp.StompMessage;
import bgu.spl.net.srv.ConnectionHandler;
import bgu.spl.net.srv.Connections;

public interface StompMessagingProtocol {
    /**
     * Used to initiate the current client protocol with it's personal connection ID and the connections implementation
     **/
    //Initiate the protocol with the active connections	structure of the server,
    //and saves the owner client's connectionId.
    void start(int connectionId, Connections<String> connections);

    //Process the message.
    //Responses are sent via the connections object send function.
    void process(String message);

    /**
     * @return true if the connection should be terminated
     */
    boolean shouldTerminate();

    StompMessage createError(String error, String receipt);

    StompMessage createMessage(String body, String messageIDstr, String destination);

    StompMessage createReceipt(String receiptID);

    int getSubID(StompMessage stompMessage);

}