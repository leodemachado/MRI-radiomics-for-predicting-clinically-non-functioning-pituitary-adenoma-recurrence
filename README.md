# MRI-radiomics-for-predicting-clinically-non-functioning-pituitary-adenoma-recurrence

This repository contains all the codes and instructions of the experiments related to the reseach entittled 
"MRI radiomics for the prediction of recurrence in patients with clinically non-functioning pituitary macroadenomas",
that was authored by me and colleagues, and published in the Computers in Bilogy and Medicine Journal, by Elsevier ([DOI: 10.1016/j.compbiomed.2020.103966](https://www-sciencedirect.ez67.periodicos.capes.gov.br/science/article/pii/S0010482520302997?via%3Dihub)).

### You'll find the following folders:
### CLassification-Models Folder:
- This folder contains the notebooks used for model train, testing, tuning, and evaluation.
### Conventional-Statistics-Test Folder:
- This folder contains the univariate convencional statistics analysis performed for separating recurrent from stable lesion patients.
### Descriptive-Outcome-Group-Statistics Folder:
- This folder contains the anonimized clinical and demographic data for each patient considered in the study.
### Radiomic-Feature-Analysis Folder:
- This folder contain the .csv files containing the radiomic features extracted both in the 3D approach and in the two 2D-approach considered.
The first 2D approache considered a single slice, the slice containing the largest sample of tumor found using the "Slice-Selector-And-Image-Normalize-Folder" available, for Radiomics Analysis.
The other 2D approach considered not just one, but three slices from three different heights along tumor axis for radiomic feature    extraction.
### Slice-Selector-And-Image-Normalizer Folder:
- This folder contains the C++ modeule built using ITK for finding the slice containgin the largest 2D tumor sample and image rescaling.
### Radiomics-Feature-Extration-Using-PyRadiomics Folder:
- This folder contains the .py script for extracting Radiomics Features using the PyRadiomics Library and the .yaml file containing the **setings used in the extraction process.

I hope those codes and instructions can help you to understand my research.
Best.
