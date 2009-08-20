/*
 * isisRegistration.cxx
 *
 *  Created on: July 13, 2009
 *      Author: tuerke
 */



#include "extRegistration/isisRegistrationFactory3D.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImage.h"


//via command parser include
#include "viaio/option.h"
#include "viaio/mu.h" //this is required for VNumber

VDictEntry TYPMetric[] = {
		{ "MattesMutualInformation", 0},
		{ "NormalizedHistogrammMutualInformation", 1 },
		{ "NormalizedCorrelation", 2 },
		{ NULL }
};

VDictEntry TYPTransform[] = {
		{ "Rigid", 0},
		{ "Affine", 1},
		{ "NonLinear", 2},
		{ "QuaternionRigid", 3},
		{ "EulerRigid", 4},
		{ NULL }
};

//command line parser options
VString ref_filename = NULL;
VString in_filename = NULL;
VString out_filename = NULL;
VShort number_of_bins = 50;
VShort number_of_iterations = 200;
VBoolean transformVersorRigid = false;
VBoolean transformQuaternionRigid = false;
VBoolean transformEulerRigid = false;
VBoolean metricNormalizeMutualInformation = false;
VBoolean metricMattesMutualInformation = false;
static VShort metricType = 0;
static VShort transformType = 0;

static VOptionDescRec options[] = {
		//required inputs
		{"ref", VStringRepn, 1, &ref_filename, VRequiredOpt, 0,
		"the fixed image filename" },
		{"in", VStringRepn, 1, &in_filename, VRequiredOpt, 0,
		"the moving image filename" },
		{"out", VStringRepn, 1, &out_filename, VRequiredOpt, 0,
		"the output image filename" },
		//parameter inputs
		{"bins", VShortRepn, 1, &number_of_bins, VOptionalOpt, 0,
		"Number of bins used by the MattesMutualInformationMetric to calculate the image histogram"	},
		{"iter", VShortRepn, 1, &number_of_iterations, VOptionalOpt, 0,
		"Maximum number of iteration used by the optimizer" },
		//component inputs
		{"metric",	VShortRepn, 1, (VPointer) &metricType, VOptionalOpt, TYPMetric, "Type of the metric"},
		{"transform", VShortRepn, 1, (VPointer) &transformType, VOptionalOpt, TYPTransform, "Type of the transform"}
};

int main( int argc, char* argv[] )
{
	VParseCommand( VNumber(options), options,  & argc, argv );




	typedef short InputPixelType;
	typedef short OutputPixelType;
	const unsigned int Dimension = 3;

	typedef itk::Image< InputPixelType, Dimension > FixedImageType;
	typedef itk::Image< InputPixelType, Dimension > MovingImageType;
	typedef itk::Image< OutputPixelType, Dimension > OutputImageType;

	typedef itk::ImageFileReader< FixedImageType > FixedImageReaderType;
	typedef itk::ImageFileReader< MovingImageType > MovingImageReaderType;
	typedef itk::ImageFileWriter< OutputImageType > WriterType;

	typedef isis::RegistrationFactory3D< FixedImageType, MovingImageType > RegistrationFactoryType;

	FixedImageReaderType::Pointer fixedReader = FixedImageReaderType::New();
	MovingImageReaderType::Pointer movingReader = MovingImageReaderType::New();
	WriterType::Pointer writer = WriterType::New();

	fixedReader->SetFileName( ref_filename );
	movingReader->SetFileName( in_filename );
	writer->SetFileName( out_filename );

	fixedReader->Update();
	movingReader->Update();


	RegistrationFactoryType::Pointer registrationFactory = RegistrationFactoryType::New();
	registrationFactory->SetInterpolator( RegistrationFactoryType::Linear );


	//transform setup
	std::cout << "used transform: " << TYPTransform[transformType].keyword << std::endl;
	switch (transformType)
	{
	case 0:
		registrationFactory->SetTransform( RegistrationFactoryType::VersorRigid3DTransform );
		break;
	case 3:
		registrationFactory->SetTransform( RegistrationFactoryType::QuaternionRigidTransform );
		break;
	case 4:
		registrationFactory->SetTransform( RegistrationFactoryType::CenteredEuler3DTransform );
		break;
	}

	//metric setup
	std::cout << "used metric: " << TYPMetric[metricType].keyword << std::endl;
	switch (metricType)
	{
	case 0:
		registrationFactory->SetMetric( RegistrationFactoryType::MattesMutualInformation );
		break;

	case 1:
		registrationFactory->SetMetric( RegistrationFactoryType::NormalizedMutualInformation );
		break;



	}




	registrationFactory->SetOptimizer( RegistrationFactoryType::VersorRigidOptimizer );
	registrationFactory->UserOptions.NumberOfIterations = number_of_iterations;
	registrationFactory->UserOptions.NumberOfBins = number_of_bins;
	registrationFactory->UserOptions.PRINTRESULTS = true;

	registrationFactory->SetFixedImage( fixedReader->GetOutput() );
	registrationFactory->SetMovingImage( movingReader->GetOutput() );
	registrationFactory->StartRegistration();
	writer->SetInput( registrationFactory->GetRegisteredImage() );
	writer->Update();














	return 0;


}