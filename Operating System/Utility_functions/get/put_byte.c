typedef  sizeof(int)/2  word
typedef u16 




word get_word(){
    word word;
    u16 ds = getds();   //getds in asse,bly return DS value

    setds(segment);
    // Now you are in DS location you specified 
    word = *(word *) offset;
    setds(ds);  //setds() in assembly restores DS
    return word;
}

void put_word(word word, word segement, word offset){
    word ds = getds();      //save DS

    setds(segement);        //set DS to segment
    *(word *)offset = word;

    setds(ds);          //restore DS
}