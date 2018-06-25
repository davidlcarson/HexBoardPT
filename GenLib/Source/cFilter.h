#pragma once

//cFilter.h

#define FILTER_PI  double (3.1415926535897932384626433832795)
#define FILTER_2PI double (2.0 * FILTER_PI)
#define FILTER_4PI double (4.0 * FILTER_PI)

#include "globdefs.h"
#include <math.h>

enum eFILTERTYPE{
 eBOXFILTER_3,
 eBILINEAR_3,
 eGAUSSIAN_3,
 eHAMMING_3,
 eBLACKMAN_3
};

//--------------------------------------------------------------------------------+
class cGENERIC_FILTER
{
 protected:
   double m_dWidth;
    
 public:    
    cGENERIC_FILTER (double dWidth){ m_dWidth = dWidth;}
    virtual ~cGENERIC_FILTER(void) {}

    double GetWidth(void ){ return m_dWidth; }
    void   SetWidth (double dWidth){ m_dWidth = dWidth; }

    virtual double Filter (double dVal) = 0;
};

//--------------------------------------------------------------------------------+
class cBOX_FILTER_3 : public cGENERIC_FILTER
{
 private:

 public:
    cBOX_FILTER_3 (double dWidth = double(0.5)) : cGENERIC_FILTER(dWidth) {}
    virtual ~cBOX_FILTER_3(void) {}

    double Filter (double dVal){ return (fabs(dVal) <= m_dWidth ? 1.0 : 0.0); }
};

//--------------------------------------------------------------------------------+
class cBILINEAR_FILTER_3 : public cGENERIC_FILTER
{
 private:

 public:
    cBILINEAR_FILTER_3 (double dWidth = double(1.0)) : cGENERIC_FILTER(dWidth) {}
    virtual ~cBILINEAR_FILTER_3(void){}

    double Filter (double dVal) 
        {
            dVal = fabs(dVal); 
            return (dVal < m_dWidth ? m_dWidth - dVal : 0.0); 
        }
};

//--------------------------------------------------------------------------------+
class cGAUSSIAN_FILTER_3 : public cGENERIC_FILTER
{
 private:

 public:
    cGAUSSIAN_FILTER_3 (double dWidth = double(3.0)) : cGENERIC_FILTER(dWidth) {}
    virtual ~cGAUSSIAN_FILTER_3(void) {}

    double Filter (double dVal) 
        {
            if (fabs (dVal) > m_dWidth) 
            {
                return 0.0;
            }
            return exp (-dVal * dVal / 2.0) / sqrt (FILTER_2PI); 
        }
};

//--------------------------------------------------------------------------------+
class cHAMMING_FILTER_3 : public cGENERIC_FILTER
{
 private:

 public:
    cHAMMING_FILTER_3 (double dWidth = double(0.5)) : cGENERIC_FILTER(dWidth) {}
    virtual ~cHAMMING_FILTER_3(void) {}

    double Filter (double dVal) 
        {
            if (fabs (dVal) > m_dWidth) 
            {
                return 0.0; 
            }
            double dWindow = 0.54 + 0.46 * cos (FILTER_2PI * dVal); 
            double dSinc = (dVal == 0) ? 1.0 : sin (FILTER_PI * dVal) / (FILTER_PI * dVal); 
            return dWindow * dSinc;
        }
};

//--------------------------------------------------------------------------------+
class cBLACKMAN_FILTER_3 : public cGENERIC_FILTER
{
 private:

 public:
    cBLACKMAN_FILTER_3 (double dWidth = double(0.5)) : cGENERIC_FILTER(dWidth) {}
    virtual ~cBLACKMAN_FILTER_3(void) {}

    double Filter (double dVal) 
        {
            if (fabs (dVal) > m_dWidth) 
            {
                return 0.0; 
            }
            double dN = 2.0 * m_dWidth + 1.0; 
            return 0.42 + 0.5 * cos (FILTER_2PI * dVal / ( dN - 1.0 )) + 
                   0.08 * cos (FILTER_4PI * dVal / ( dN - 1.0 )); 
        }
};
  

