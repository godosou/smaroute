#include "patTransportMode.h"


patString getTransportMode( TransportMode m){
	switch(m){
	case WALK:
		return "walk";
	case BIKE:
		return "bike";
	case BUS:
		return "bus";
	case METRO:
		return "metro";
	case TRAIN:
		return "train";
	case CAR:
		return "car";
	default:
		return "none";
	}
}

bool isPublicTransport(TransportMode m){
	if (m==BUS||m==TRAIN||m==METRO){
		return true;
	}
	else{
		return false;
	}
}
patString getTransportModeString( TransportMode m){
	switch(m){
	case WALK:
		return "walk";
	case BIKE:
		return "bike";
	case BUS:
		return "bus";
	case METRO:
		return "metro";
	case TRAIN:
		return "train";
	case CAR:
		return "car";
	default:
		return "none";
	}
}
