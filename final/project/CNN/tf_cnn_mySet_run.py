from tf_cnn_mySet_predict import testImages, getResizedImage
import tensorflow as tf
import os

def saveToCSV(filename, digitsList, rows):
    with open(filename, 'w') as csvFile:
        index = 0
        for count in rows:
            for i in range(count):
                csvFile.write(str(digitsList[index]))
                if i != count - 1:
                    csvFile.write(',')
                index += 1
            csvFile.write('\n')

def saveAllToCSV(filename, digitsList, imgRows):
    with open(filename, 'w') as csvFile:
        csvFile.write('filename,studentID,phone,citizenID\n')
        index = 0
        for i in range(len(imgRows)):
            csvFile.write('%d.jpg,'%i)
            row = imgRows[i]
            if len(row) >= 1:
                for i in range(row[0]):
                    csvFile.write(str(digitsList[index]))
                    index += 1
            if len(row) >= 2:
                csvFile.write(',')
                for i in range(row[1]):
                    csvFile.write(str(digitsList[index]))
                    index += 1
            if len(row) >= 3:
                csvFile.write(',')
                for i in range(row[2]):
                    csvFile.write(str(digitsList[index]))
                    index += 1
            for i in range(3, len(row)):
                index += row[i]
            csvFile.write('\n')
            

def main(argv):
    if len(argv) < 3:
        print('Invalid parameters')
        return
    lower = int(argv[1])
    higher = int(argv[2])
    fileCount = higher - lower
    print('Start CNN Prediction on %d Images'%fileCount)
    #Records how many digits in each row in every image
    imgRows = []
    with open('segmentations/digitsLayout.txt') as layouts:
        for line in layouts:
            imgRows.append([int(n) for n in line.split()][1:])
    #All filenames based on digits layout        
    filenameList = []
    resizedImages = []
    for i in range(len(imgRows)):
        for k in range(len(imgRows[i])):
            for j in range(imgRows[i][k]):
                filenameList.append(os.path.join('segmentations', '%d-%d-%d.jpg'%(i+lower, k+1, j+1)))
    for name in filenameList:
        resizedImages.append(getResizedImage(name))
    results = testImages(resizedImages)
    #Save digits to folders based on result
    index = 0
    for i in range(len(imgRows)):
        startIndex = index
        for k in range(len(imgRows[i])):
            for j in range(imgRows[i][k]):
                filename = os.path.join('CNNResult', str(results[index]), '%d-%d-%d.jpg'%(i+lower, k+1, j+1))
                resizedImages[index].save(filename)
                index += 1
        CSVFilename = os.path.join('CNNResult/xlsx', '%d.csv'%(i+lower))
        saveToCSV(CSVFilename, results[startIndex:index], imgRows[i])
    saveAllToCSV("CNN-result.csv", results, imgRows)
    print('CNN Prediction Completed.')
        
    
    

if __name__ == '__main__':
    tf.app.run()
