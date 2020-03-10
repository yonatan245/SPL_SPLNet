package bgu.spl.net.impl.stomp;

import bgu.spl.net.srv.ConnectionHandler;
import bgu.spl.net.srv.Connections;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.WeakHashMap;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentLinkedQueue;
import java.util.concurrent.CopyOnWriteArrayList;
import java.util.concurrent.atomic.AtomicInteger;

public class StompConnections implements bgu.spl.net.srv.Connections<String> {

    //Fields
    private Map<Integer, ConnectionHandler> connectionHandlers;
    private Map<Integer, User> usersByID;
    private Map<String, User> usersByName;
    private Map<String, Map<User, Integer>>topics; //Map of: genre name -> (Map of: subscription id -> user)

    //Holder
    private static class ConnectionHolder{
        private static StompConnections instance = new StompConnections();

        public static StompConnections getInstance() {
            return instance;
        }
    }

    //Constructor
    private StompConnections(){
        usersByID = new ConcurrentHashMap<>();
        usersByName = new ConcurrentHashMap<>();
        topics = new ConcurrentHashMap<>();
        connectionHandlers = new ConcurrentHashMap<>();
    }

    public static Connections getInstance(){ return ConnectionHolder.getInstance();}

    //Methods
    /**
     * If it's a new user, subscribes it into the data base.
     * If user exists, check authentication.
     * @param userName
     * @param password
     */
    public void login(String userName, String password, int connectionID) throws Exception {
        User user = usersByName.get(userName);

        if(user == null){
            user = new User(userName, password, connectionID);
            usersByName.put(userName, user);
        }

        if (!user.authenticate(userName, password)) throw new Exception("Wrong password");
        if (user.isActive()) throw new Exception("User already active");
        else user.setActive(true);


        usersByID.put(connectionID, user);
        user.setConnectionID(connectionID);
    }

    public void joinTopic(int connectionId, String genre, int subscriptionId) throws Exception {
        User user = usersByID.get(connectionId);

        if (user == null) throw new Exception("User does not exist");

        topics.putIfAbsent(genre, new ConcurrentHashMap<>());

        topics.get(genre).put(user, subscriptionId);
        user.subscribe(genre, subscriptionId);
    }

    public void exitTopic(int connectionId, int subscriptionId){
        User user = usersByID.get(connectionId);
        String genre = user.unSubscribe(subscriptionId);

        topics.get(genre).remove(user, subscriptionId);
    }

    public void addConnection(int connectionID, ConnectionHandler handler){
        connectionHandlers.put(connectionID, handler);
    }

    @Override
    public boolean send(int connectionId, String msg) {
        if(connectionHandlers.containsKey(connectionId)) {
            connectionHandlers.get(connectionId).send(msg);
            return true;
        }

        return false;
    }

    @Override
    public void send(String channel, String msg) {
        if(topics.containsKey(channel)){
            for(Map.Entry<User, Integer> entry : topics.get(channel).entrySet()){
                User user = entry.getKey();
                Integer subscriptionID = entry.getValue();

                StompMessage message = new StompMessage(msg);
                message.setHeader("subscription", subscriptionID.toString());

                send(user.getConnectionID(), message.toString());
            }
        }
    }

    @Override
    public void disconnect(int connectionId){
        User user = usersByID.get(connectionId);

        if(user != null) {

            for (String topic : topics.keySet()) topics.get(topic).remove(user);

            usersByID.remove(connectionId);

            user.disconnect();
        }
    }

}
