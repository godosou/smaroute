patOdSet::patOdSet(){


}		

set<patOd>* patOdSet::getOdSet(){
	return &odSet;
}

patOd* patOdSet::addOd(patOd theOd){
	pair<set<patOd>::iterator,bool> insertResult = odSet.insert(theOd);
	return &(*(insertResult.first()));
}

patPath* patOdSet::findPath(const patPath& thePath){
	
	patOd pathOd = thePath.getOd();
	
	patOd* odFound = findOd(pathOd);
	
	if(odFound == NULL){
		return NULL;
	}
	
	patPath* pathFound = odFound->findPath(thePath);
	
	if(pathFound == NULL){
		return NULL;
	}
	
	return pathFound;

}

patOd* patOdSet::findOd(const patOd& theOd){

	set<patOd>::iterator odFound = odSet.find(theOd);
		if(odFound == odSet.end()){
		return NULL;
	}
	
	return &(*odFound);

}
