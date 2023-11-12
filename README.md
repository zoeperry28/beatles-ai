# beatles-ai
Based on an extremely large (private) database of vocal performances from all 4 of the Beatles.

# Features
The software will have these 4 main features:
- Recognising the voice of each Beatle in any given track
- Beatle voice synthesis
- Beatles song cover generation

The dataset that I am using is private as to not infringe upon the copyrights of any third parties and will not, under any circumstance, be distributed.

# Data Generation
To run this yourself, you will first have to generate some audio data. This data should be obtained from legitimate sources. This can be done using the python script found in beatles-ai/data/training_data.

The typical usage of the script is as follows:

``` python main.py -i ../../../Imagine -o ../john ```

Sicne this uses Spleeter to extract the vocal element of the audio given, it can take quite a while.

Once the data has been generated, you will be able to use it to train the model.
