#this script is meant to read in the emg and accelerometer data from a text file, filter the data, averages each emg signal,
# then saves the filtered data to filtered_data.txt and saves the average emg signal for each sensor to mean_emg.txt

import math 
import numpy as np
import scipy 
from scipy.signal import butter
from scipy.signal import lfilter
from scipy.signal import freqz
from scipy.signal import find_peaks
import matplotlib.pyplot as plt

#import data from text file
#infile = open('0_C_E2.txt', 'r')  #read from the input file
#line = infile.readlines()[4:]
#infile.close()

infile = np.genfromtxt('test_data_file.txt')


emg1, emg2, emg3, accx, accy, accz = [], [], [], [], [], []
emg1 = np.array([float (row[0]) for row in infile])
emg2 = np.array([float (row[1]) for row in infile])
emg3 = np.array([float (row[2]) for row in infile])
accx = np.array([float (row[3]) for row in infile])
accy = np.array([float (row[4]) for row in infile])
accz = np.array([float (row[5]) for row in infile])

a1 = accx[:10000] #cut accx to 30 seconds
a2 = accy[:10000] #Cut accy to 30 seconds
a3 = accz[:10000] #cut accz to 30 seconds
e1 = emg1[:10000] #cut emg1 to 30 seconds
e2 = emg2[:10000] #cut emg2 to 30 seconds
e3 = emg3[:10000] #cut emg3 to 10 seconds


# Write a function to process EMG
sampling_rate = 1000 #1000 samples per second (Hz)

#convert the raw emg signal to mV
e1 = (((e1/1034-0.5)*3.3)/1009)*1000 
e2 = (((e2/1034-0.5)*3.3)/1009)*1000
e3 = (((e3/1024-0.5)*3.3)/1009)*1000 
time = np.arange(0.001, 10.001, 0.001) #for plotting with time

# bandpass butterworth filter (20-350Hz), rectified signal for emg
Band = np.dot((2/1000),[20, 350]) #bandpass - low end: 20 mV, high end: 350 mV (to eliminate Gaussian noise)
B, A = scipy.signal.butter(2, Band, 'Bandpass') #second order butterworth bandpass filter
emg1_filt = scipy.signal.filtfilt(B, A, e1)
emg2_filt = scipy.signal.filtfilt(B, A, e2)
emg3_filt = scipy.signal.filtfilt(B, A, e3)

#Butterworth filter for ACC data
Fn = sampling_rate / 2
passband = np.array([5.0, 20])/Fn
stopband = np.array([0.1, 40])/Fn 
passripple = 1
stopripple = 10

C, D = scipy.signal.buttord(passband, stopband, passripple, stopripple, analog=False,fs=None)
acc1_filt = scipy.signal.filtfilt(D, C, a1)
acc2_filt = scipy.signal.filtfilt(D, C, a2)
acc3_filt = scipy.signal.filtfilt(D, C, a3)
"""#combine the triaxial accelerometer data into a single array
vector = np.vectorize(np.int_)
y = np.column_stack((acc1_filt, acc2_filt, acc3_filt))
x1 = vector(y[0])
x2 = vector(y[1])
x3 = vector(y[2])
x = vector(y)
acc_filt = math.sqrt(x[0]**2 + x[1]**2 + x[2]**2)"""

#Save the filtered data to a text file
filtered_data = np.column_stack((acc1_filt, emg1_filt, emg2_filt, emg3_filt))
np.savetxt('filtered_data.txt', filtered_data, delimiter='\t')

emg1_mean = abs(np.mean(emg1_filt))
emg2_mean = abs(np.mean(emg2_filt))
emg3_mean = abs(np.mean(emg3_filt))


mean_data = np.column_stack((emg1_mean, emg2_mean, emg3_mean))
np.savetxt('mean_emg.txt', mean_data, delimiter='\t')
#np.array2string(mean_data, max_line_width=None, precision=None, suppress_small=None, separator='    ', prefix=None, sign=None, floatmode=None)



#detect peaks in filtered emg signals
"""emg3_peaks, _ = scipy.signal.find_peaks(emg3_filt, height=0)
plt.plot(emg3_peaks, emg3_filt[emg3_peaks], "x")
plt.plot(np.zeros_like(emg3_filt), "--", color="gray")
plt.show()"""


    


#np.ndarray.tofile('mean_emg.txt',int(emg1_mean), sep='\n', format='%f')

#Generate the power spectrum of filtered EMG values 
#Power Spectrum of EMG1 filtered
f = 1000*(np.arange(0, 10000/2+1))/10000
Y1 = np.fft.fft(emg1_filt)
PA = np.abs(Y1/10000)
P1 = PA[:int(10000/2+1)]
P1 = 2*P1
#Power Spectrum of EMG2 filtered
Y2 = np.fft.fft(emg2_filt)
PB = np.abs(Y2/10000)
P2 = PB[:int(10000/2+1)]
P2 = 2*P2
#Power Spectrum of EMG3 filtered
Y3 = np.fft.fft(emg3_filt)
PC = np.abs(Y3/10000)
P3 = PC[:int(10000/2+1)]
P3 = 2*P3

Y4 = np.fft.fft(acc1_filt)
PD = np.abs(Y4/10000)
P4 = PD[:int(10000/2+1)]
P4 = 2*P4

#Save Power Spectrums of EMG Sensors to Text File
power_spectrum_data = np.column_stack((P1, P2, P3, P4))
np.savetxt('power_spectra.txt', power_spectrum_data, delimiter='\t')

#average power spectrum of filtered accelerometer date and save it to a text file
acc1_P_mean = np.mean(P4)
emg1_P_mean = np.mean(P1)
emg2_P_mean = np.mean(P2)
emg3_P_mean = np.mean(P3)
mean_power_spectrum_data = np.column_stack((acc1_P_mean, emg1_P_mean, emg2_P_mean, emg3_P_mean))
np.savetxt('mean_power_spectrum_data.txt', mean_power_spectrum_data, delimiter='\t')


plt.figure()
plt.subplot(3,2,1)
plt.plot(time, emg3_filt)
plt.title('MVC EMG3 Filtered')
plt.xlabel('Time(s)')
plt.ylabel('EMG Amplitude(mV)')
plt.xticks([0,4,8,10])
plt.xlim([0,10])

plt.subplot(3,2,2)
plt.plot(time, e3)
plt.title('MVC EMG3 Unfiltered')
plt.xlabel('Time(s)')
plt.ylabel('EMG Amplitude(mV)')
plt.xticks([0,4,8,10])
plt.xlim([0,10]) 

plt.subplot(3,2,5)
plt.plot(f, P4)
plt.title('Accelerometer Power Spectrum')
plt.xlabel('f (Hz)')
plt.ylabel('Acceleromter (m/s^2)')
plt.xticks([0,100,200,300,400,500])
#plt.xlim([0,10])
#plt.ylim([0,0.1])

plt.subplot(3,2,6)
plt.plot(f,P3)
plt.title('Power Spectrum of MVC Emg3')
plt.xlabel('f (Hz)')
plt.ylabel('|P1(f)|')
plt.ylim([0,0.01])
plt.xlim([0,300])



plt.show()

