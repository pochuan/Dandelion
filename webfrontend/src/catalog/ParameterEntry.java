package catalog;

import java.util.Random;

import testing.RandomStringUtils;

public class ParameterEntry {
	private int messageID;
	private int announce;
	private String name;
	int length; //array length of parameter
	typeID type;
	//Flags
	boolean isWritable; //User can change value
	boolean isNonvolatile; //Parameter is flash backed
	boolean isTransient;//The car changes this parameter
	public enum typeID{
		UINT8,
		INT8, 
		UINT16, 
		UINT32, 
		INT32,
		CHAR, 
		FLOAT,
		DOUBLE, 
		BOOLEAN, 
		UINT32_ENUM
	}
	public ParameterEntry(int messageID, int length, int announce, String name, typeID type, int flags){
		this.messageID = messageID;
		this.length = length;
		this.announce = announce;
		this.name = name;
		this.type = type;
	}
	
	//testing code for generating random stuff for the frontend. 
	public static ParameterEntry randomEntry(){
		Random gen = new Random();
		int messageID = gen.nextInt(255);
		int length = gen.nextInt(255);
		int announce = gen.nextInt(1000);
		String name = RandomStringUtils.randomAlphabetic(10);
		typeID type = typeID.values()[gen.nextInt(9)];
		return new ParameterEntry(messageID, length, announce, name, type, 0);
	}
}
