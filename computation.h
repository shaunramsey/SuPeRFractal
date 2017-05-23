#pragma once

#include<string>


//no longer used .. . . deprecate
double computeR(int n, std::string& ab, double a, double b) {
	int len = ab.length();
	if (ab[n%len] == 'A')
		return a;
	else
		return b;
}

//no longer required....deprecated
double computeX(double X, int n, std::string& ab, double a, double b) {

	for (int i = 1; i <= n; i++) {
		X = computeR(i - 1, ab, a, b) * X * (1 - X);
	}
	return X;
}
//no longer used- deprecate
double computeX(int n, std::string& ab, double a, double b) {

	double X = 0.5;
	for (int i = 1; i <= n; i++) {
		X = computeR(i - 1, ab, a, b) * X * (1 - X);
	}
	return X;
}

//unused...deprecate
double computeXrecursively(int n, std::string ab, double a, double b) {
	if (n == 0)
		return 0.5;
	else {
		double X = computeXrecursively(n - 1, ab, a, b);
		return computeR(n - 1, ab, a, b) * X * (1 - X);
	}

}


//if X and Y diverge, that means that some element close to X got much further away thus .. chaos? hmmm
double lambdaEPSILON(double a, double b, std::string&ab, int N) {
	double EPSILON = 0.00001;
	double R, X = 0.5, Y = X + EPSILON, DX=0;
	for (int i = 0; i < N; i++) {
		for (unsigned int j = 0; j < ab.length(); j++) {
			R = ab[j] == 'A' ? a : b;
			X = R * X * (1 - X); //whatever your function is here...
			Y = R * Y * (1 - Y);
			DX = fabs(Y - X); 
			if (DX > 10) { i = N; break; }
		}
	}
	double L = log(DX / EPSILON) / (N*ab.length());
	return L;
}


//X may float around for awhile and then become either constant or consistently bounce between some values
//definitely would love to see a graph of this. Probably need to fire up some maple/mathematica/whatevs
double WarmupX(double a, double b, std::string& ab) {
	double R, X = 0.5;
	int len = (int)ab.length();
	int NW = 400; // number of warm ups
	for (int i = 0; i < NW; i++) {
		for (int j = 0; j < len; j++) { // go through the sequence only once - maybe this is just how they do the modulus for finding the value?
			R = ab[j] == 'A' ? a : b;
			X = R * X * (1 - X);
			if (fabs(X) > 3) { return 100; }
		}
	}
	return X;
}

//do the warmup and then compute the derivs. 
double lambdaWarmupReal(double a, double b, std::string& ab, int N) {
	double X = WarmupX(a, b, ab);
	if (X > 50) { return 3; } 
	if (X == 0.5) { return -INFINITY; }
	double sum = 0;
	unsigned int len = ab.length();
	for (int i = 1; i <= N; i++) {
		double R = ab[i%len] == 'A' ? a : b;
		double f = fabs(R * (1 - 2 * computeX(X, i, ab, a, b))); //don't need to compute X from scratch...
		if (f == 0) return -INFINITY; //quick out
		sum = sum + log(f);
	}
	sum = sum / N;
	return sum; // the lambda
}

//warmup X....but if X stops changing, nuke it. 
//if f is ever 0 . . then this must mean X is 1/2, so...- INFIN?
//otherwise get the value and then then color according to ..
double lambdaWarmup(double a, double b, std::string& ab, int N) {
	int NW = 600; //warm up rounds
	double X = 0.5, R;
	int len = (int)ab.length();
	for (int i = 0; i < NW; i++) {
		for (int j = 0; j < len; j++) {
			R = ab[j] == 'A' ? a : b;
			X = R * X * (1 - X);
			if (fabs(X) > 3) { return 2; }
		}
	}
	double sum = 0, f;
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < len; j++) {
			R = ab[j] == 'A' ? a : b;
			X = R * X * (1 - X);
			if (fabs(X) > 3) { return 2; }
			f = R - 2 * R * X;
			if (f == 0) { return -INFINITY; }
			sum = sum + log(fabs(f));
		}
	}
	sum = sum / (len*N);
	return sum;

}



double lambdaReal(double a, double b, std::string& ab, int N) {
	
	double sum = 0;
	for (int i = 1; i <= N; i++) {

		sum = sum + log(fabs(computeR(i, ab, a, b) * (1 - 2 * computeX(i, ab, a, b))));
	}
	sum = sum / N;
	return sum; // the lambda
}


double oldX(double a, double b, std::string&ab, int N) {

	double adjustedX = a;
	double rn = 4.0;
	for (int i = 0; i < N * 100; i++) {
		adjustedX = rn * adjustedX * (1 - adjustedX);
	}
	return adjustedX - 2;
}


double newX(double a, double b, std::string&ab, int N) {
	if (a < 0) a = 0;
	if (b < 0) b = 0;
	double adjustedX = 0.5;
	//std::cout << "startadj=" << adjustedX;
	for (int i = 0; i < N*100; i++) {
		adjustedX = (exp(a)  * adjustedX * ((exp(a) - 1) * adjustedX + -b*adjustedX + b + 1)) /
			pow(((exp(a) - 1) * adjustedX + 1), 2.0);
		//std::cout << ", " << adjustedX;
	}
	return adjustedX - 2;
}

//warmup X....but if X settles into a constant, nuke it. 
//if f is ever 0 . . then this must mean return - INFIN
//otherwise get the value and then then color according to ..
//this function produces the images in our paper
double lambdaSuPeRWarmup(double a_in, double b_in, std::string& ab, int N, double aval) {
	int NW = 10; //warm up rounds, more is slightly better
	N = N * 1;
	double X = 0.5;
	double a = aval, b = 0;
	double one = 1.0; //this value should be an actual 1.0
	int len = (int)ab.length();
	for (int i = 0; i < NW; i++) { //bounce around teh function. hopefully X settles down
		for (int j = 0; j < len; j++) {
			b = ab[j] == 'A' ? a_in : b_in;
			X = (exp(a)  * X * ((exp(a) - 1) * X + -b*X + b + 1)) / pow(((exp(a) - 1) * X + 1), 2.0);
		//	X = b * X * (1 - X) + one * X; // X * (b * (-X) + b + one);
			// if (fabs(X) > 3) { return 2; }
		}
	}
	double sum = 0, f;
	for (int i = 0; i < N; i++) { //how many iterations? defined by N only
		for (int j = 0; j < len; j++) { //iterates over AABB string
			b = ab[j] == 'A' ? a_in : b_in;
			X = (exp(a)  * X * ((exp(a) - 1) * X + -b*X + b + 1)) / pow(((exp(a) - 1) * X + 1), 2.0);
			//X = b* X * (1 - X) + one * X;// X * (b * (-X) + b + one);
		//	if (fabs(X) > 3) { return 2; }
			f = -(exp(a) * (b * (exp(a)*X + X - 1) - exp(a)*X + X - 1)) / ( pow( (exp(a)-1)*X + 1, 3.0));
		//	f = b - 2 * b* X + one;
			if (f == 0) { return -INFINITY; }
			sum = sum + log(fabs(f));
		}
	}
	sum = sum / (len*N);
	return sum;

}

//the lambda function that is actually called.
//allows us to use one of many lambda functions written
//according to the value of lock
double lambda(double a, double b, std::string&ab, int N, double aval, int lock) {
	switch (lock) {
	case 0:
		return oldX(a, b, ab, N);
		break;
	case 1:
		return newX(a, b, ab, N);
		break;
	case 2:
		return lambdaWarmup(a, b, ab, N);
		break;
	case 3:
		return lambdaSuPeRWarmup(a, b, ab, N, aval);
		break;
	default:
		return lambdaSuPeRWarmup(a, b, ab, N, aval);
		break;
	}

	
}