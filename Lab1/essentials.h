#include<string.h>
#include<string>
const int maxMessageLength = 1024;

//so that we can split by this complex separator 
//and get type of message and content of message
const std::string messageSeparator = "$$$###$$$";
const std::string ack = "ACK";


//--------------------------Base64 Encode Decode---------------------------------
static const std::string base64_chars =
             "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
             "abcdefghijklmnopqrstuvwxyz"
             "0123456789+/";

//given base64 encoded character, it returns the index of that character in 6 bit form
//Basically in the array of base64_chars to get the original 
//6 bits position from which the character was mapped
std::bitset<24> getEncodingPosition(char c){
    std::bitset<24> ans;
    //if capital letters then it's simply the distance from 'A'
    if(c >= 'A' && c <= 'Z')
        ans = c-'A';
    else if(c >= 'a' && c <= 'z')//it's simply distance from 'a' + 26
        ans = 26 + c-'a';
    else if(c >= '0' && c <= '9')//it's simply distance from '0' + 52
        ans = 52 + c-'0';
    else
        ans = (c =='+' ? 62 : 63);
    return ans;
}

std::string encodeBase64(std::string& str){
    std::string encodedString = "";
    std::bitset<24> threeCharGrp;
    std::bitset<24> mask = 63;
    for(int i = 0; i < str.size(); i++){

        if(i > 0 && i%3==0){
            //getting the first six bits and converting to base64
            encodedString.push_back(base64_chars[(((mask<<18)&threeCharGrp)>>18).to_ulong()]);

            //getting the second set of six bits and converting to base64
            encodedString.push_back(base64_chars[(((mask<<12)&threeCharGrp)>>12).to_ulong()]);

            //getting the third set of six bits and converting to base64
            encodedString.push_back(base64_chars[(((mask<<6)&threeCharGrp)>>6).to_ulong()]);

            //getting the last set of six bits and converting to base64
            encodedString.push_back(base64_chars[(mask&threeCharGrp).to_ulong()]);            

            threeCharGrp.reset();
        }
        
        //collecting 3 bytes into threeCharGrp
        threeCharGrp <<= 8;
        threeCharGrp |= (int8_t)str[i];
    } 

    if(str.size()%3 == 0){
        //remaining last three char grp
        //getting the first six bits and converting to base64
        encodedString.push_back(base64_chars[(((mask<<18)&threeCharGrp)>>18).to_ulong()]);
        //getting the second set of six bits and converting to base64
        encodedString.push_back(base64_chars[(((mask<<12)&threeCharGrp)>>12).to_ulong()]);
        //getting the third set of six bits and converting to base64
        encodedString.push_back(base64_chars[(((mask<<6)&threeCharGrp)>>6).to_ulong()]);
        //getting the last set of six bits and converting to base64
        encodedString.push_back(base64_chars[(mask&threeCharGrp).to_ulong()]);            
    }

    else if(str.size()%3 == 2){
        //remaining group of two chars (if stringsize%3 == 2)
        //pad 2 bits to the end to make 18 bits and so we don't have first set of bits here
        threeCharGrp <<= 2;
        //getting the second set of six bits and converting to base64
        encodedString.push_back(base64_chars[(((mask<<12)&threeCharGrp)>>12).to_ulong()]);
        //getting the third set of six bits and converting to base64
        encodedString.push_back(base64_chars[(((mask<<6)&threeCharGrp)>>6).to_ulong()]);
        //getting the last set of six bits and converting to base64
        encodedString.push_back(base64_chars[(mask&threeCharGrp).to_ulong()]);            

        //for stringsize%3 == 2, we padd '=' to show that one set of 6 bits is not there
        encodedString.push_back('=');
    }
    

    else{
        //remaining group of single char (if stringsize%3 == 1)
        //pad 4 bits to the end to make 12 bits and so we don't have first and second set of bits here
        threeCharGrp <<= 4;
        //getting the third set of six bits and converting to base64
        encodedString.push_back(base64_chars[(((mask<<6)&threeCharGrp)>>6).to_ulong()]);
        //getting the last set of six bits and converting to base64
        encodedString.push_back(base64_chars[(mask&threeCharGrp).to_ulong()]);            

        //for stringsize%3 == 1, we padd '==' to show that two set of 6 bits is not there
        encodedString.append("==");
    }
    return encodedString;
}

std::string decodeBase64(std::string str){
    if(str.size()%4 != 0)   //every encoded string have length in multiples of 4
        return "";

    std::string decodedString = "";
    std::bitset<24> fourEncodedCharGrp;
    std::bitset<24> mask = 255;
    for(int i = 0; i < str.size(); i++){

        if(i > 0 && i%4==0){
            //getting the first eight bits and decoding it to ascii
            decodedString.push_back((char)(((mask<<16)&fourEncodedCharGrp)>>16).to_ulong());

            //getting the second set of eight bits and decoding to ascii
            decodedString.push_back((char)(((mask<<8)&fourEncodedCharGrp)>>8).to_ulong());

            //getting the third set of eight bits and decoding to ascii
            decodedString.push_back((char)((mask&fourEncodedCharGrp)).to_ulong());

            fourEncodedCharGrp.reset();
        }
        
        //collecting 4 encoded chars for decoding to 3 ascii chars
        fourEncodedCharGrp <<= 6;
        fourEncodedCharGrp |= getEncodingPosition(str[i]);
    } 

    //decoding last set of 4 encoded chars
    int removeBits = 0;
    if(str.back() == '='){
        fourEncodedCharGrp >>= 6; //removing padded '=' from the set
        removeBits += 2;   //remove two bits from end
        str.pop_back();
    }
    if(str.back() == '='){
        fourEncodedCharGrp >>= 6; //removing padded '=' from the set
        removeBits += 2;   //remove two bits from end
        str.pop_back();
    }

    fourEncodedCharGrp >>= removeBits;

    //all fourEncodedCharGrp must be decoded to 3 ascii character
    if(removeBits == 0){
        //getting the first eight bits and decoding it to ascii
        decodedString.push_back((char)(((mask<<16)&fourEncodedCharGrp)>>16).to_ulong());

        //getting the second set of eight bits and decoding to ascii
        decodedString.push_back((char)(((mask<<8)&fourEncodedCharGrp)>>8).to_ulong());

        //getting the third set of eight bits and decoding to ascii
        decodedString.push_back((char)((mask&fourEncodedCharGrp)).to_ulong());
    }
    else if(removeBits == 2){//two ascii character will be decoded
        //getting the first of two character and decoding to ascii
        decodedString.push_back((char)(((mask<<8)&fourEncodedCharGrp)>>8).to_ulong());
        //getting the third set of eight bits and decoding to ascii
        decodedString.push_back((char)((mask&fourEncodedCharGrp)).to_ulong());
    }
    else{   //one ascii character only rest was padding
        decodedString.push_back((char)((mask&fourEncodedCharGrp)).to_ulong());
    }

    return decodedString;
}


//creates message to send over network
//stores that message in char array message with size of array = len
//given by caller of function 
//also checks that message doesn't exceeds the 1024 bytes limit 
//message will look like type|separator|content
bool createMessage(int type, const std::string& mssg, char *message, int len){

    std::string messageToSend = std::to_string(type) + messageSeparator + mssg;
    
    //encode the message
    messageToSend = encodeBase64(messageToSend);

    if(messageToSend.length()+1 > len){
        return false;   //message larger than size of buffer
    }
    int i = 0;
    for(auto& c: messageToSend){
        message[i++] = c;
    }
    message[i] = '\0';  //end with null character
    
    return true;
}

//analyze the message in form of type + separator + message
std::pair<int, std::string> analyseMessage(char *buffer, int bufferSize){
    
    std::pair<int, std::string> result = std::make_pair(-1, "");
    std::string mssg(buffer);
    
    mssg = decodeBase64(mssg);

    auto loc = mssg.find(messageSeparator);
    if(loc == std::string::npos)    //invalid message as separator must be present
        return result;

    //message type extraction
    result.first = stoi(mssg.substr(0, loc));

    //message content extraction
    result.second = mssg.substr(loc+messageSeparator.length());
    
    return result;
}

//return valid port value from args or else returns -1 
int fetchPortValue(char *arg){
    int port = -1;     
    try{
        port = std::stoi(arg);
    }catch(std::exception& e){
        //if some error occurs return -1
        std::cerr<<e.what()<<std::endl;
    }
    return (port > 0 && port < 1<<16) ? port : -1;
}