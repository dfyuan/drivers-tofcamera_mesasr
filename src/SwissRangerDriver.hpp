#ifndef _SWISSRANGERDRIVER_HPP_
#define _SWISSRANGERDRIVER_HPP_

#include <iostream>
#include <cstdlib>
#include <stdexcept>
#include <vector>
#include <algorithm>
#include <base/logging.h>
#include <base/samples/pointcloud.h>
#include <base/samples/frame.h>
#include <boost/lexical_cast.hpp>
#include <boost/range.hpp>

#include "libMesaSR.h"

#include "SwissRangeDriverLookUp.hpp"
#include "SwissRangerTypes.hpp"

#define TIMEOUT 3000

namespace tofcamera_mesa_swissranger
{
    
    /**
     * \short This class implements a driver for the TOF-camera MESA Swissranger
     * \author Anna Born <anna.born@dfki.de>
     * \note The driver was tested with MESA Swissranger SR-4000.
     * \note All function except getLibraryVersion(std::string &version) should be called after the device is open.
     * \note API for sensor could be found under http://www.mesa-imaging.ch/dlm.php?fname=./customer/driver/Swissranger-HTML-doc1.0.12.x.zip
     *
     */
    class SwissRangerDriver
    {
        public:
            SwissRangerDriver();
            ~SwissRangerDriver();

            /**
             * \short Opens the Swissranger on USB port.
             * \brief The serial number is only used, if more than one camera is connected to the computer.
             * Setting the serial_number to 0 opens the first camera.
             * \param serial_number: the last 8 digits of the printed serial number, as a hexadecimal value,
             * e.g. 0x4000002f for the camera serial number: 00-00-40-00-00-2F. (s. sensor API)
             * \return true if the camera was opened successfully, otherwise false
             */
            bool openUSB(const unsigned int serial_number);

            /**
             * \short Opens the Swissranger on Ethernet port.
             * \param ip_address: the ip address of the device, e.g. "192.168.2.14"
             * \return true if the camera was opened successfully, otherwise false
             * \note This function was not tested
             */
            bool openEthernet(const std::string &ip_address);

            /**
             * \short Checks if the Swissranger device is open.
             * \return true if the device is open, false if it is closed
             */
            bool isOpen() const;

            /**
             * \short Closes the Swissranger camera.
             * \return true if the camera was closed successfully, otherwise false
             */
            bool close();

            /**
              * \short Returns the version of the libMesaSR library.
              * \param[out] version: library version
              * \return true if the library information could be accessed, otherwise false
              */
            bool getLibraryVersion(std::string &version);
            
            /**
             * \short Returns the information about the Swissranger device.
             * \param[out] info: device information
             * (format: "VendorID: '0xXXXX', ProductID: '0xXXXX', Manufacturer: 'XXX', Product: 'XXX'")
             * \return true if the device information could be accessed, otherwise false
             */
            bool getDeviceInfo(std::string &info);

            /**
             * \short Sets acquisition mode
             * \brief This function overwrites the current acquisition mode.
             * (e.g. default mode for SR4k is AM_COR_FIX_PTRN|AM_CONV_GRAY|AM_DENOISE_ANF
             * (s. sensor API))
             * \param mode: s. sensor documentation for all available acquisition modes
             * \return true if the acquisition mode was set successfully, otherwise false
             * \note To add/remove one mode to/from the current acquisition mode
             * use the function turnAcquisitionMode(AcquireMode mode, bool isOn).
             *
             */
            bool setAcquisitionMode(int mode);

            /**
             * \short Returns acquisition mode
             * \param[out] mode: s. sensor documentation for all available acquisition modes
             * \return true if the acquisition mode information could be accessed, otherwise false
             * \note To get the description of a acquisition mode use
             * acquireModesToStr(int mode, std::string &mode_str).
             *
             */
            bool getAcquisitionMode(int &mode);

            /**
             * \short Adds/removes modes to/from the current acquisition mode
             * \param mode: s. sensor documentation for all available acquisition modes
             * \param turnOn: true - turn on, false - turn off
             * \return
             */
            bool turnAcquisitionMode(int mode, bool turnOn);

            /**
             * \short Sets timeout for a reading and writting on USB/Ethernet port.
             * \param ms: time in ms
             */
            void setTimeout(int ms);

            /**
             * \return time in ms
             */
            int getTimeout();

            /**
             * \short Returns the number of rows that the camera delivers
             * \param[out] rows - number of rows of sensor image
             * \return true if the number of rows could be accessed, otherwise false
             */
            bool getRows(unsigned int &rows);

            /**
             * \short Returns the number of cols that the camera delivers
             * \param[out] cols - number of cols of sensor image
             * \return true if the number of cols could be accessed, otherwise flase
             */
            bool getCols(unsigned int &cols);

            /**
             * \short Sets integration time for the device. (s. sensor API)
             * \param time - integration time in range from 0 to 255.
             * \return
             * \note (e.g. for SR-4k the integration time is 0.300ms+(time)*0.100 ms)
             */
            bool setIntegrationTime(unsigned char time);
            bool getIntegrationTime(unsigned char &time);

            /**
             * \short Sets the amplitude threshold
             * \brief It the amplitude value is lower than threshold, than the distance value will be set to 0.
             * Default value is 0.
             * \param threshold
             * \return
             */
            bool setAmplitudeThreshold(unsigned short threshold);
            bool getAmplitudeThreshold(unsigned short &threshold);

            bool setModulationFrequency(ModulationFrq frequency);
            bool getModilationFrequency(ModulationFrq &frequency);

            /**
             * \short Sets auto exposure (s. sensor API)
             * \param min_int_time - to turn of auto exposure set the value to 255
             * \param max_int_time
             * \param percent_over_pos
             * \param desired_pos
             * \return
             */
            bool setAutoExposure(unsigned char min_int_time, unsigned char max_int_time,
                                 unsigned char percent_over_pos, unsigned char desired_pos);

            /**
             * \short Triggers the camera acqusition
             * \return
             */
            bool acquire();

            /**
             * \short Returns the raw distance image
             * \brief (s. sensor manual for composition of distance value)
             */
            bool getDistanceImage(base::samples::frame::Frame &frame);

            /**
             * \short Returns the amplitude image
             * \brief (s. sensor manual for composition of amplitude value)
             * \return
             */
            bool getAmplitudeImage(base::samples::frame::Frame &frame);

            /**
             * \short Returns the confidence map image
             * \brief The value represents the reliability of the distance measurement.
             * The greater z-value represents the higher confidence.
             * \return
             */
            bool getConfidenceMapImage(base::samples::frame::Frame &frame);

            bool isConfidenceImageAvailable();

            /**
             * \short Returns the distance measurement in cartesian coordinaten.
             * \brief CP_DOUBLE and CP_FLOAT returns the distance in meter, CP_UINT in millimeter
             * According the sensor API the CP_DOUBLE and CP_FLOAT have the same precision.
             * Accordint to sensor API: Z increases away from the camera, Y upwards,
             * and X to the left, from the camera's viewpoint, i.e. a Right Handed coordinate system.
             * The origin (0,0,0) is the intersection of the optical axis and the front face of the camera.
             */
            bool getDistanceCartesian(base::samples::Pointcloud &pointcloud, const TMS_CoordPercision persicion);


        private:
            CMesaDevice* camera_handle_;
            bool is_open_;

            /** \short pointer to ImgEntry array that contains
             *  all available images (e.g. distance image, amplitude image)
             */
            ImgEntry *img_entry_array_;

            /**
             * \brief holds indexes to the images in img_entry_array_
             * The values in img_indexes are corresponded to the type of enum ImgType (s. SwissRanger API)
             *
             * img_indexes_[0] - index to the image of the type IT_DISTANCE
             * img_indexes_[1] - index to the image of the type IT_AMPLITUDE
             * ...
             * img_indexes_[8] - index to the image of the type IT_CONF_MAP
             * ...
             *
             * If some value in img_indexes is -1, there is no index of the image with the corresponded type.
             * Therefore there is no image in img_entry_array_ of this type.
             */
            std::vector<int> img_indexes_;

            /** \short number of available images */
            int number_of_images_;

            unsigned int rows_;
            unsigned int cols_;

            int timeout_;

            void* image_buffer_;

            /**
             * \short gets the image information from the device
             * \return true if all information could be accessed, otherwise false
             * \note this function is called after the opening the device
             */
            bool init();

            /**
             * \brief update the image information from the device
             * \return
             * \note this function should be called after
             * the changes in acquisition mode applied
             */
            bool initImageList();

            /**
             * \short warn if the acquisition mode is just for internal testing
             * \brief s. API of the device
             * \param mode
             */
            void warnInternalAcquisitionMode(int mode);

            /**
             * \short warn if the modulation frequency is just for internal testing
             * \brief s. API of the device
             */
            void warnInternalModulationFrequency(ModulationFrq frequency);

            bool getImage(base::samples::frame::Frame &frame, const int index);
	};

} // end namespace tofcamera_mesa_swissranger

#endif // _SWISSRANGERDRIVER_HPP_