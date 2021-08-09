/*=========================================================================
 *
 *  Copyright Insight Software Consortium
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkExtractImageFilter.h"
#include "itkStatisticsImageFilter.h"

#include "itkLabelMap.h"
#include "itkLabelObject.h"
#include "itkLabelImageToLabelMapFilter.h"
#include "itkLabelMapMaskImageFilter.h"

#include "itkN4BiasFieldCorrectionImageFilter.h"

#include "itkRescaleIntensityImageFilter.h"

// for filepath accessing
#include "string"             // for std::string
#include "cstddef"            // for std::size_t

#include "math.h"

#include "itkCastImageFilter.h"

#include "itkMultiplyImageFilter.h"

int main( int argc, char ** argv )
{
  // Verify the number of parameters in the command line
  if( argc <= 2 )
    {
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << " input3DImageFile  input3DLabelImageFile " << std::endl;
    return EXIT_FAILURE;
    }

  using PixelType = unsigned short;
  using OutputPixelType = float;
  const unsigned int Dimension = 3;

  using ImageType = itk::Image< PixelType, Dimension >;
  using OutputImageType = itk::Image< OutputPixelType, Dimension>;

  using ReaderType = itk::ImageFileReader< ImageType >;
  using WriterType = itk::ImageFileWriter< OutputImageType >;

  ImageType::Pointer featureImage = ImageType::New();

  ReaderType::Pointer reader1 = ReaderType::New();
  reader1->SetFileName(argv[1]);

  ReaderType::Pointer reader2 = ReaderType::New();
  ImageType::Pointer labelImage = ImageType::New();
  reader2->SetFileName(argv[2]);


  using BiasFilterType = itk::N4BiasFieldCorrectionImageFilter< ImageType, ImageType >;
  BiasFilterType::Pointer biasFilter = BiasFilterType::New();
  biasFilter->SetInput1( reader1->GetOutput() );
  biasFilter->SetMaskImage( reader2->GetOutput() );
  biasFilter->SetUseMaskLabel( true );

  try
    {
    biasFilter->Update();
    std::cerr << " BiasFilter correction done! " << std::endl;
    }
  catch( itk::ExceptionObject & err )
    {
    std::cerr << "ExceptionObject caught !" << std::endl;
    std::cerr << err << std::endl;
    return EXIT_FAILURE;
    }

  featureImage = biasFilter->GetOutput();
  labelImage = reader2->GetOutput();

  // Here we recover the file names from the command line arguments
  // catching path out of Imagefilename

  std::string fullname = argv[1];
  std::string filepath = fullname.substr(0,fullname.find_last_of("\\/"));
  std::string fileWithExtension = fullname.substr(fullname.find_last_of("\\/")+1);
  std::string fileName = fileWithExtension.substr(0,fileWithExtension.find_last_of("."));
  fileName = fileName.substr(0,fileWithExtension.find_last_of("-"));

  std::cout<<" Patient ID extraction done! "<<std::endl;

  // creating LabelStatisticsFilter to find the biggest slice.
  using StatisticsImageFilterType = itk::StatisticsImageFilter< ImageType >;
  StatisticsImageFilterType::Pointer StatisticsFilter = StatisticsImageFilterType::New();

  // defining extract filter type and extract filter object
  using ExtractFilterType = itk::ExtractImageFilter< ImageType, ImageType >;
  ExtractFilterType::Pointer extractFilterLabel = ExtractFilterType::New();
  extractFilterLabel->SetDirectionCollapseToSubmatrix();
  extractFilterLabel->SetInput( labelImage );

  // set up the extraction region [one slice]
  // Planes of slice extraction:
  // 0 - Coronal
  // 1 - Axial
  // 2 - Sagital

  // set up the extraction region [one slice]

  const unsigned int slicePlane = 0; // Coronal
  unsigned int sliceNumber = 0; // First slice checked
  ImageType::RegionType inputRegion = labelImage->GetBufferedRegion();
  ImageType::SizeType size = inputRegion.GetSize();

  // Catching the number of slices in the plane direction extracted
  const unsigned int numOfSlices = static_cast<unsigned int>(size[slicePlane]);
  std::cout<<" Number of Slices: "<< numOfSlices <<std::endl;

  size[slicePlane] = 1; // The dimension perpendicular to the desired plane is set to one
  ImageType::RegionType desiredRegion;
  desiredRegion.SetSize(  size  );

  ImageType::IndexType start = inputRegion.GetIndex();

  // label slice summation and index
  unsigned int sliceSum = 0;
  unsigned int biggestSlice = 0;
  unsigned int width = 0;

  for (sliceNumber = 0; sliceNumber < numOfSlices; sliceNumber++){

      start[slicePlane] = sliceNumber;
      desiredRegion.SetIndex( start );
      extractFilterLabel->SetExtractionRegion( desiredRegion );

      StatisticsFilter->SetInput( extractFilterLabel->GetOutput() );
      StatisticsFilter->Update();

      unsigned int sum = static_cast<unsigned int>(StatisticsFilter->GetSum());

      if (sum != 0){
          width += 1;
      }

      if (sum > sliceSum) {
          sliceSum = sum;
          biggestSlice = sliceNumber;
      }
  }

  std::cout<<" Search for the biggest slice done! "<<std::endl;
  std::cout<<" Biggest Slice: "<< biggestSlice <<". Width: "<<width<<std::endl;

  // We will extract the following slices:
  unsigned int firstSlice = static_cast<unsigned int>(round(biggestSlice - 0.25*width));
  //unsigned int firstSlice = 96;
  unsigned int midSlice = static_cast<unsigned int>(biggestSlice);
  unsigned int lastSlice = static_cast<unsigned int>(round(biggestSlice + 0.25*width));

  std::cout<<" Selected Slices: "<<firstSlice<<", "<<midSlice<<", "<<lastSlice<<std::endl;

  using MultiplyImageFilterType = itk::MultiplyImageFilter< ImageType, ImageType >;
  MultiplyImageFilterType::Pointer multiplyFilter = MultiplyImageFilterType::New();

  using RescFilterType = itk::RescaleIntensityImageFilter< ImageType, ImageType >;
  RescFilterType::Pointer rescFilterImage = RescFilterType::New();

  using CastFilterType = itk::CastImageFilter< ImageType, OutputImageType >;
  CastFilterType::Pointer castFilterImage = CastFilterType::New();
  CastFilterType::Pointer castFilterLabel = CastFilterType::New();
  CastFilterType::Pointer castFilterImageNorm = CastFilterType::New();

  WriterType::Pointer writer = WriterType::New();

  // Range values to normalize all the images
  unsigned short min = 1.0;
  unsigned short max = 128.0;

  ////////////////// // Generating normal image:
  ImageType::Pointer normImage = ImageType::New();

  multiplyFilter->SetInput1(featureImage);
  multiplyFilter->SetInput2(labelImage);

  rescFilterImage->SetOutputMinimum( min );
  rescFilterImage->SetOutputMaximum( max );
  rescFilterImage->SetInput(multiplyFilter->GetOutput());
  rescFilterImage->Update();
  normImage = rescFilterImage->GetOutput();

  castFilterImageNorm->SetInput(rescFilterImage->GetOutput());
  writer->SetFileName( filepath + "/" + fileName + "-norm.nrrd" );
  writer->SetInput( castFilterImageNorm->GetOutput() );
  writer->Update();

  // Instantiating an extractor object to work with the normalized image
  ExtractFilterType::Pointer extractFilterImage = ExtractFilterType::New();
  extractFilterImage->SetDirectionCollapseToSubmatrix();
  extractFilterImage->SetInput( normImage );

  // ////////////////////////////////////////////////////////////////////
  // Extracting biggest featureImage slice

  // Storing Biggest Slice-label:

  start[slicePlane] = midSlice;
  desiredRegion.SetIndex( start );
  desiredRegion.SetSize( size );

  // Extracting Slices from image and label

  extractFilterImage->SetExtractionRegion( desiredRegion );
  extractFilterLabel->SetExtractionRegion( desiredRegion );

  // Cropping feature image with label.

  multiplyFilter->SetInput1( extractFilterImage->GetOutput() );
  multiplyFilter->SetInput2( extractFilterLabel->GetOutput() );

  // Casting

  castFilterImage->SetInput( multiplyFilter->GetOutput() );

  // Saving

  writer->SetFileName( filepath + "/" + fileName + "-bigstSlice.nrrd" );
  writer->SetInput( castFilterImage->GetOutput() );
  writer->Update();

  castFilterLabel->SetInput( extractFilterLabel->GetOutput() );
  writer->SetFileName( filepath + "/" + fileName + "-bigstSlice-label.nrrd" );
  writer->SetInput( castFilterLabel->GetOutput() );
  writer->Update();

  std::cout<<" MID labelImage2D done! "<<std::endl;

  return EXIT_SUCCESS;
}

