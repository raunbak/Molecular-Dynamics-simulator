#include "stdafx.h" // Pre-compiled header.
#include "FastEnsemble.h"
#include "FastIon.h"
#include "constants.h"
#include <iostream>

#include <stdarg.h>


using namespace std;

// Constructor
FastEnsemble::FastEnsemble(int mass, int N)
{
	SteadyStateTemperature = SteadyStateTzSec;
	NumberOfIons = N;
	ions = new FastIon [NumberOfIons];

	for(int n = 0; n < NumberOfIons; n++)
	{
		ions[n].initialize(mass);
	}
}

FastEnsemble::FastEnsemble(int m1, int n1, int m2, int n2)
{
	SteadyStateTemperature = SteadyStateTzSec;
	NumberOfIons=n1+n2;
	ions = new FastIon [NumberOfIons];

	for(int n = 0; n < NumberOfIons; n++) {
		if (n<n1 ){
		
  		ions[n].initialize(m1); // setting mass, secular velocity and allocating memory for the ions pos and vel arrays
		}
		else {
		
  		ions[n].initialize(m2); // setting mass, secular velocity and allocating memory for the ions pos and vel arrays
		}
	}
}
	
// Member functions
void FastEnsemble::CleanUpEnsemble()
{
	for(int n = 0; n  < NumberOfIons; n++)
		ions[n].CleanUpIon();


	for(int i = 0; i < HistNx;i++)
	{
		for(int j = 0; j < HistNy;j++)
		{
			delete [] histogram[i][j];
			delete [] VelHistogram[i][j];
		}

		delete [] histogram[i];
		delete [] VelHistogram[i];
	}
	delete histogram;
	delete VelHistogram;

}

double FastEnsemble::GetCurrentTemperature()
{
	return (pow(VzSecRMS,2)+pow(VrSecRMS,2))*Mass(0)/Kb/3; // REMEBER TO CHANGE THIS FOR MASSES
}

void FastEnsemble::RescaleVelocityXYZ(double Total_V_x_rms,double Total_V_y_rms,double Total_V_z_rms)
{
		// calculate current temperature
	double CurrentTempZ = (pow(Total_V_z_rms,2)*Mass(0))/(Kb);
	double CurrentTempX = (pow(Total_V_x_rms,2)*Mass(0))/(Kb);
	double CurrentTempY = (pow(Total_V_y_rms,2)*Mass(0))/(Kb);

	// rescale velocity distribution (Maybe change which fraction of SST is given)
	 
	cout << sqrt(pow(CurrentTempZ,2)+pow(CurrentTempX,2) + pow(CurrentTempY,2) )<< endl;
	double T_fraction = SteadyStateTemperature;

	double az = sqrt((T_fraction)/CurrentTempZ);
	double ax = sqrt((T_fraction)/CurrentTempX);
	double ay = sqrt((T_fraction)/CurrentTempY);

	cout << ax <<" " << ay << " " << az << endl; 

	double lowerLimit = 0.98;//sqrt(100/102.5); // Hvorn�r der ikke skal k�les mere.
	double upperLimit = 1.02;//sqrt(100/97.5); // Hvorn�r der ikke skal varmes op mere.

	//cout << lowerLimit << " " << upperLimit << endl;
	// Good value for raise temp is 1.005

		if (ax > upperLimit)
		{
			ax =1.0000002;
		} 
		else if (ax < lowerLimit)
		{
			ax = 0.9990;
		}
		else {
			ax = 1.0;
		}

		if (ay > upperLimit)
		{
			ay =1.0000002;
		}
		else if (ay < lowerLimit)
		{
			ay = 0.9990;
		}
		else {

			ay = 1.0;
		}

		if (az > upperLimit)
		{
			az = 1.0000002;
		}
		else if (az < lowerLimit)
		{
			az = 0.9990;
		}
		else {
			az = 1.0;
		}
		
		/*
		if (ax < lowerLimit)
		{
			ax = 0.9990;
		}
		else {

			if ( ReachedTempArea == true)
			{
			ax = 0.9999;
			ReachedTempArea = false;
			}
			else
			{
			ax = 1.0001;
			ReachedTempArea = true;
			}
		}

		if (ay < lowerLimit)
		{
			ay = 0.9990;
		}
		else {

			if ( ReachedTempArea == true)
			{
			ay = 0.9999;
			ReachedTempArea = false;
			}
			else
			{
			ay = 1.0001;
			ReachedTempArea = true;
			}
		}

		if (az < lowerLimit)
		{
			az = 0.9990;
		}
		else {
			if ( ReachedTempArea == true)
			{
			az = 0.99999;
			ReachedTempArea = false;
			}
			else
			{
			az = 1.00001;
			ReachedTempArea = true;
			}
		}
		
		if (ax < lowerLimit)
		{
			ax = 0.9990;
		}
		else {

			ax = 1.0000;
		}

		if (ay < lowerLimit)
		{
			ay = 0.9990;
		}
		else {

	       ay = 1.0000;
		}

		if (az < lowerLimit)
		{
			az = 0.9990;
		}
		else {
			az = 1.0000;
		}
		*/
		cout <<"ax = " << ax << "  ay = " << ay << " az= " << az  << endl;

	for(int n = 0; n < NumberOfIons; n++)
	{
		ions[n].SetVelocity(0, ions[n].Velocity(0)*ax);
		ions[n].SetVelocity(1, ions[n].Velocity(1)*ay);
		ions[n].SetVelocity(2, ions[n].Velocity(2)*az);
	}
}

void FastEnsemble::InitialiseVelocityHistogram()
{
	//allocating memory to histogram
	VelHistogram = new long double ** [HistNx];
	for(int i = 0; i < HistNx;i++)
	{
		VelHistogram[i] = new long double *[HistNy];
		for(int j = 0; j < HistNy;j++)
			VelHistogram[i][j] = new long double [HistNz];
	}
	for(int i=0; i < HistNx; i++)
		for(int j=0; j < HistNy; j++)
			for(int k=0; k < HistNz; k++)
				VelHistogram[i][j][k] = 0;
}

void FastEnsemble::InitialiseCountHistogram()
{
	//allocating memory to histogram
	CountHistogram = new long int ** [HistNx];
	for(int i = 0; i < HistNx;i++)
	{
		CountHistogram[i] = new long int *[HistNy];
		for(int j = 0; j < HistNy;j++)
			CountHistogram[i][j] = new long int [HistNz];
	}
	for(int i=0; i < HistNx; i++)
		for(int j=0; j < HistNy; j++)
			for(int k=0; k < HistNz; k++)
				CountHistogram[i][j][k] = 0;
}

void FastEnsemble::InitialiseHistogram()
{
	//allocating memory to histogram
	histogram = new long int ** [HistNx];
	for(int i = 0; i < HistNx;i++)
	{
		histogram[i] = new long int *[HistNy];
		for(int j = 0; j < HistNy;j++)
			histogram[i][j] = new long int [HistNz];
	}
	for(int i=0; i < HistNx; i++)
		for(int j=0; j < HistNy; j++)
			for(int k=0; k < HistNz; k++)
				histogram[i][j][k] = 0;

}

void FastEnsemble::UpdateHistogram()
{
	for (int i = 0; i < NumberOfIons; i++)
	{
		int Nx = ((int) ((ions[i].Position(0))/PixelToDistance+((double) HistNx)/2));
		int Ny = ((int) ((ions[i].Position(1))/PixelToDistance+((double) HistNy)/2));
		int Nz = ((int) ((ions[i].Position(2))/PixelToDistance+((double) HistNz)/2));

		if (Nx < HistNx && Ny < HistNy && Nz < HistNz && Nx > 0 && Ny > 0 && Nz > 0)
		{
			histogram[Nx][Ny][Nz]++;
		}
	}
}

void FastEnsemble::MyUpdateVelocityHistogram()
{
	for (int i = 0; i < NumberOfIons; i++)
	{
		int Nx = ((int) ((ions[i].Position(0))/PixelToDistance+((double) HistNx)/2));
		int Ny = ((int) ((ions[i].Position(1))/PixelToDistance+((double) HistNy)/2));
		int Nz = ((int) ((ions[i].Position(2))/PixelToDistance+((double) HistNz)/2));

		if (Nx < HistNx && Ny < HistNy && Nz < HistNz && Nx > 0 && Ny > 0 && Nz > 0)
		{
			VelHistogram[Nx][Ny][Nz]+= ((long double) pow(ions[i].Velocity(),2));
		}
	}
}

void FastEnsemble::UpdateCountHistogram()
{
	for (int i = 0; i < NumberOfIons; i++)
	{
		int Nx = ((int) ((ions[i].Position(0))/PixelToDistance+((double) HistNx)/2));
		int Ny = ((int) ((ions[i].Position(1))/PixelToDistance+((double) HistNy)/2));
		int Nz = ((int) ((ions[i].Position(2))/PixelToDistance+((double) HistNz)/2));

		if (Nx < HistNx && Ny < HistNy && Nz < HistNz && Nx > 0 && Ny > 0 && Nz > 0)
		{
			CountHistogram[Nx][Ny][Nz]++;
		}
	}
}

double FastEnsemble::ReturnHist(int i, int j, int k)
{
	return histogram[i][j][k];
}

double FastEnsemble::ReturnVelHist(int i, int j, int k)
{
	return ((double) VelHistogram[i][j][k]);
}

double FastEnsemble::ReturnCountHist(int i, int j, int k)
{
	return ((double) CountHistogram[i][j][k]);
}

void FastEnsemble::SetSteadyStateTemperature(double Val)
{
	SteadyStateTemperature = Val;
}

int FastEnsemble::GetNumberOfIons()
{
	return NumberOfIons;
}

double FastEnsemble::Mass(int N)
{
	return ions[N].GetMass();
}

double FastEnsemble::getRho0(double Vrf){

	return eps0*Vrf*Vrf/(Mass(0)*pow(r0,4)*OmegaRF*OmegaRF);
}

void FastEnsemble::CrystalGenerator(double Vrf, double Vend)
{
	// Calculating pseudo trap frequencies
	double wz2=2*eta*e*Vend/pow(z0,2)/Mass(0); // ERROR in formula from Magnus' thesis! he means potential and not electric potential.
	double wr2=pow(e*Vrf/Mass(0)/OmegaRF,2)/2/pow(r0,4)-eta*e*Vend/Mass(0)/pow(z0,2);

	// Solving plasma model aspect-ratio equation

	double x0 = 0.001;
	double xn = 100.0;
	double eps = 0.0001;
	BisectionRootFinder Roots;
	Roots.SetWsquare(wz2/wr2);

	double alpha = Roots.bisection(x0,xn,eps);//CalculateAspectRatio(wz2/wr2);
	//cout << "alpha " << alpha <<" and ratio is "<< wz2/wr2  << endl;
	//double alpha = CalculateAspectRatio(2);
	//cout << "aspect ratio is " << alpha << '\n';
	/*cout << "out putting ratio(alpha)\n";
	for (double ratio = 0.01; ratio < 2; ratio+=0.01)
		if (ratio != 1)
			cout << CalculateAspectRatio(ratio*ratio) << ';' << '\n';
	 */
	// Calculating number density
	double rho0 = (eps0*Vrf*Vrf)/(Mass(0)*pow(r0,4)*OmegaRF*OmegaRF);

	// Calculating volume of crystal
	double V = ((double) NumberOfIons)/rho0;
	//cout << "volume of crystal is " << V << '\n';

	// Calculating length and radius of crystal
	double L = pow(6*V/(PI*alpha*alpha), ((double) 1) /  ((double) 3));
	bool CrystalNotDone = true;
	while(CrystalNotDone)
	{
		double R = alpha*L/2;

		//cout << "radius is " << R << '\n';
		//cout << "Length is " << L << '\n';
		//cout << "2R/L=" << 2*R/L << '\n';

		// unit cell cube length for bcc structure
		double a = pow( 2 / rho0, ((double) 1) / ((double) 3));

		//cout << "unit cell length is " << a << '\n';

		//cout << "Number of ions " << NumberOfIons << '\n';

		int IonNumber = 0;
		for(int i = -1*((int) ceil(R/a)); i <= ((int) ceil(R/a));i++)
			for(int j = -1*((int) ceil(R/a)); j <= ((int) ceil(R/a));j++)
				for(int k = -1*((int) ceil(L/a/2)); k <= ((int) ceil(L/a/2));k++)
				{
					// placing ion in unit cell "origo" corner and checking if all ions have a position
					if((IonNumber < NumberOfIons) && (pow(a*((double) i)/R,2) + pow(a*((double) j)/R,2) + pow(a*((double) k)*2/L,2) <= 1)) //if ion is inside ellipsoid
					{
						ions[IonNumber].SetPosition(0, ((double) i)*a);
						ions[IonNumber].SetPosition(1, ((double) j)*a);
						ions[IonNumber].SetPosition(2, ((double) k)*a);
						IonNumber++;
					}


					// placing ion in unit cell center and checking if all ions have a position
					if((IonNumber < NumberOfIons) && (pow(a*(((double) i) + 0.5)/R,2) + pow(a*(((double) j) + 0.5)/R,2) + pow(a*(((double) k) + 0.5)*2/L,2) <= 1)) //if ion is inside ellipsoid
					{
						ions[IonNumber].SetPosition(0, (((double) i) + 0.5)*a);
						ions[IonNumber].SetPosition(1, (((double) j) + 0.5)*a);
						ions[IonNumber].SetPosition(2, (((double) k) + 0.5)*a);
						IonNumber++;
					}

				}

		if(IonNumber < NumberOfIons)
		{
			//cout << NumberOfIons - IonNumber << " ion(s) have not been positioned\n";
			L=L*1.005; // increasing crystal length with 0.5 percent
		}
		else
		{
			Radius = R;
			Length =L;
			//cout << "All ions have been positioned\n";
			CrystalNotDone=false;
		}


		//cout << a << " a_WS" <<endl;
	}

	// NORMALT ER DEN NUL! LIGE NU SVARER DEN TIL 1K !
	// setting all ions to zero-velocity
	for (int i = 0; i < NumberOfIons; i++)
	{
		for (int dim = 0; dim < 3; dim++)
		{
			double vel = rand() % 10;
			if(vel < 5 ) {
				vel = -1.0;
			}
			else {
				vel = 1.0;
			}
			ions[i].SetVelocity(dim, vel*5.5838);
		}

	}
	cout << Radius << " = Radius of crystal " << Length << " = length of crystal"<< endl; //REMOVE THIS!!
}

double FastEnsemble::Ekin()
{
	double Ekin = 0;
	for(int n = 0; n < NumberOfIons; n++)
		Ekin += ions[n].Ekin();

	return Ekin;
}

double FastEnsemble::Ttot()
{
	return Ekin()/(1.5*NumberOfIons*Kb);
}

double FastEnsemble::Position(int dim, int N)
{
	return ions[N].Position(dim);
}

double FastEnsemble::Velocity(int dim, int N)
{
	return ions[N].Velocity(dim);
}

void FastEnsemble::SavePositionToFile()
{
	FILE *f;

	f=fopen("MDpos.xyz","w");

	fprintf(f, "%d\n%s\n", NumberOfIons, "Ca");
	for(int N=1; N <= NumberOfIons; N++)
		fprintf(f, "%s%d\t%f\t%f\t%f\n ", "Ca", N, Position(0, N-1)*1e6, Position(1, N-1)*1e6, Position(2, N-1)*1e6);

	fclose(f);
}

void FastEnsemble::SaveIonDataToFile()
{
	// Saves ion data to file
	ofstream Ionfile ("IonData.txt");

	Ionfile << "N \t mass \t x \t y \t z \t Vx \t Vy \t Vz \t Vsec \t norm(V)" << endl; 
	Ionfile << "R: "<< Radius <<" L: " << Length << endl; 
	for(int N = 0; N < GetNumberOfIons(); N++)
	{
		if (Ionfile.is_open())
		{
			Ionfile <<			N	+ 1			<< ",  " <<
								ions[N].GetMass()		<< ",  " <<
								ions[N].Position(0)		<< ",  " <<
								ions[N].Position(1)		<< ",  " <<
								ions[N].Position(2)		<< ",  " <<
								ions[N].Velocity(0)		<< ",  " <<
								ions[N].Velocity(1)		<< ",  " <<
								ions[N].Velocity(2)		<< ",  " <<
								ions[N].Velocity()		<< ",  " <<
								endl;
		}   

	}
	Ionfile.close();
    
}


