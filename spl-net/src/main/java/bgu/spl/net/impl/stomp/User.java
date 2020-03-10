package bgu.spl.net.impl.stomp;

import bgu.spl.net.srv.ConnectionHandler;

import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentLinkedQueue;

public class User<T> {

    //Fields
    private String userName;
    private String password;
    private boolean isActive;
    private Integer connectionID; //-1 if disconnected
    private Map<Integer, String> subscribedTopics; //Map of: subcriptionID (unique) -> genre

    //Constructor
    public User(String userName, String password, int connectionID){
        this.userName = userName;
        this.password = password;
        this.connectionID = connectionID;
        subscribedTopics = new ConcurrentHashMap<>();
        isActive = false;
    }

    //Methods
    public String getUserName() {
        return userName;
    }

    public String getPassword() {
        return password;
    }

    public void setConnectionID(Integer connectionID) {
        this.connectionID = connectionID;
    }

    public boolean isActive() {
        return isActive;
    }

    public Integer getConnectionID() {
        return connectionID;
    }

    public void setActive(boolean active) {
        isActive = active;
    }

    public boolean authenticate(String userName, String password){
        return userName.equals(this.userName) && password.equals(this.password);
    }

    public void subscribe(String genre, int subscriptionID){
        subscribedTopics.put(subscriptionID, genre);
    }

    public String unSubscribe(int subscriptionID){
        return subscribedTopics.remove(subscriptionID);
    }

    public void disconnect(){
        isActive = false;
        subscribedTopics.clear();
        connectionID = -1;
    }

}
