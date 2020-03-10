package bgu.spl.net.impl.stomp;

//import javafx.util.Pair;
//
//import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;

public class StompMessage {

    //Fields
    private String headline;
    private Map<String, String> headers;
    private String body;
    private final static String END_LINE = "\u0000";


    //Constructor
    public StompMessage(){
        headers = new HashMap();
    }

    public StompMessage(String message){
        String[] messageByLines = message.split("\n");

        headline = messageByLines[0];
        headers = new HashMap<>();
        body = "";

        int line = 1;
        while(line < messageByLines.length && !messageByLines[line].equals("") && !messageByLines[line].equals(END_LINE)){
            String[] header = messageByLines[line].split(":", 2);
            if(header[1].charAt(0)==' '){
                header[1]=header[1].substring(1);
            }
            headers.put(header[0], header[1]);
            line++;
        }
        line++;

        StringBuilder messageBody = new StringBuilder();

        while(line < messageByLines.length && !messageByLines[line].equals(END_LINE)) {
            messageBody.append(messageByLines[line]).append('\n');
            line++;
        }

        if(messageBody.length() != 0) body = messageBody.substring(0, messageBody.length()-1);
    }

    public StompMessage(StompMessage message){
        headline = message.getHeadline();
        headers = message.getHeaders();
        body = message.getBody();
    }

    //Methods
    public String getHeadline() {
        return headline;
    }

    public void setHeadline(String headline) {
        this.headline = headline.toUpperCase();
    }

    public Map<String, String> getHeaders() {
        return headers;
    }

    public String getHeader(String headerName){
        return headers.get(headerName);
    }

    public String getBody() {
        return body;
    }

    public void setBody(String body) {
        this.body = body;
    }

    public void setHeader(String headerName, String value){
        if(headers.replace(headerName, value) == null) headers.put(headerName, value);
    }

    public void deleteHeader(String headerName){
        headers.remove(headerName);
    }

    public String toString(){
        String message = headline +'\n';

        for(Map.Entry<String, String> headline : headers.entrySet())
            message += headline.getKey() +": " +headline.getValue() +'\n';

        if(body != null) message += '\n' +body +'\n';
        message += END_LINE;

        return message;
    }
}
