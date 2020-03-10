package bgu.spl.net.srv;

import java.io.IOException;

public interface Connections<T> {

    //Sends a message T to client represented by the given connectionId
    boolean send(int connectionId, T msg);

    //Sends a message T to clients subscribed to the channel
    void send(String channel, T msg);

    //Removes an active client connectionId from the map
    void disconnect(int connectionId);

    //Login and subscribe if necessary
    public void login(String userName, String password, int connectionID) throws Exception;

    //Subscribe the user to desired topic
    public void joinTopic(int connectionId, String genre, int subscriptionId) throws Exception;

    //Unsubscribe the user from the desired topic
    public void exitTopic(int connectionId, int subscriptionId);

    //Adds a new connection handler to connections
    public void addConnection(int connectionID, ConnectionHandler handler);

}
