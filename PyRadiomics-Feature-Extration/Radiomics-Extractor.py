#!/usr/bin/env python

# Routine for feature extraction using feature extractor class

from __future__ import print_function

import os

import sys
import logging

import numpy as np
import SimpleITK as sitk
import six

import radiomics

# Setting up error log
# Get the PyRadiomics logger (default log-level = INFO
logger = radiomics.logger
logger.setLevel(logging.DEBUG)  # set level to DEBUG to include debug log messages in log file

# Write out all log entries to a file
handler = logging.FileHandler(filename='testLog.txt', mode='w')
formatter = logging.Formatter("%(levelname)s:%(name)s: %(message)s")
handler.setFormatter(formatter)
logger.addHandler(handler)


# Local images for one-time testing
'''
input_imageName = "/home/leonardo/Desktop/Modulos-Source/Modulos_to_ITK/SliceSelector/Build/0692604I-short.nrrd"
input_maskName = "/home/leonardo/Desktop/Modulos-Source/Modulos_to_ITK/SliceSelector/Build/0692604I-short-label.nrrd"
patient_id = "0692604I"
reader1 = sitk.ImageFileReader()
reader2 = sitk.ImageFileReader()
reader1.SetFileName(input_imageName)
reader2.SetFileName(input_maskName)
image = reader1.Execute()
mask = reader2.Execute()
'''

# Checking and reading inputs

if len(sys.argv) != 4:
   print("Usage: " + sys.argv[0] +
         "[input_imageName] [input_maskName] [patient_id]")
   sys.exit(1)

reader1 = sitk.ImageFileReader()
reader1.SetFileName(sys.argv[1])
reader1.ReadImageInformation()
image = reader1.Execute()

reader2 = sitk.ImageFileReader()
reader2.SetFileName(sys.argv[2])
reader2.ReadImageInformation()
mask = reader2.Execute()

patient_id = sys.argv[3]

# Configuring parameter file # Setting the path to the file
dataDir = "/home/leo/Desktop/Mestrado/RadiomicsShellScripts"
params = os.path.join(dataDir,"params.yaml")
#params = "~/Desktop/Mestrado/RadiomicsShellScripts/params.yaml"

############ EXECUTING FEATURE EXTRACTION
# Creating extractor object
extractor = radiomics.featureextractor.RadiomicsFeatureExtractor(params)
# Running extractor
results = extractor.execute(image, mask)

# Storing results without header information
# Additional conditions to remove non-firstorder wavelet and gradient features 
# arrays to hold feature results:

features = []
features.append("Patients")
values = []
values.append(patient_id)

# Making sure that only first order features for Wavelet and Grandient Images will be calculated
# the list "results" contain all the features extracted from the image inserted (including all the wavelets and all the gradients features)
# We were interested only in the FirstOrder features of Wavelet and Gradient filtered images.
# To achieve it, we selected them manually in the list "results".

i = 0 # feature counter 
for (key, val) in six.iteritems(results):
    
    # the results comes with a 32 lines header. The first condition is to avoid storing those information;
    if (i >= 32): 
        features.append(key) 
        values.append(val)
    i += 1 
    '''if (key.find("wavelet") == -1) and (key.find("gradient") == -1):
        # This condition means if the feature is neither wavelet nor gradient it WILL BE SAVED; 
    elif (key.find("wavelet") != -1) and (key.find("firstorder") != -1):
        # This condition means if the feature is WAVELET it MUST BE FIRSTORDER too to be SAVED;
        features.append(key) 
        values.append(val)
    elif (key.find("gradient") != -1) and (key.find("firstorder") != -1):
        # This condition means if the feature is GRADIENT it MUST BE FIRSTORDER too to be SAVED;
        features.append(key) 
        values.append(val)'''
print('done')

#  Making feature values and names list into a single string
feature_vals = ','.join(str(e) for e in values)    
feature_keys = ','.join(str(e) for e in features)  #  making list into a string

# printing the number of features extracted
print("Total number of features extrated: ", len(features), ".")


# This routine checks if the .csv file exists, inf so, it insert the features extracted from
# the current patient at the end of the file, otherwise, it creates a file, and insert 
# feature names and feature values;

'''if os.path.exists("pyRadiomicsOutput.csv"):
    with open("pyRadiomicsOutput.csv", "a+") as f:
        f.write(feature_vals)
        f.write("\n")
else:'''
with open("pyRadiomicsOutput.csv", "a+") as f:
    f.write(feature_keys) 
    f.write("\n")
    f.write(feature_vals)
    f.write("\n")