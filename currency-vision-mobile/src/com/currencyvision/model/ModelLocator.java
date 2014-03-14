package com.currencyvision.model;

public class ModelLocator {

	private static ModelLocator singleton;

	public static final int RECOGNIZE_MODE = 0;
	
	public static final int COUNT_MODE = 1;
	
	private int taskMode = 0;
	
	private CurrencyCount currencyCount = null;
	
	private ModelLocator(){
	}
	
	public static ModelLocator getInstance(){
		if(singleton == null){
			singleton = new ModelLocator();
		}
		return singleton;
	}

	public int getTaskMode() {
		return taskMode;
	}

	public void setTaskMode(int taskMode) {
		this.taskMode = taskMode;
	}

	public CurrencyCount getCurrencyCount() {
		return currencyCount;
	}

	public void setCurrencyCount(CurrencyCount currencyCount) {
		this.currencyCount = currencyCount;
	}
	
	
	
}
