#! python3
# *****************************************************************************
#
# File:                 streamlinesForMOPS.py
# Project:              python3
# Author(s):            Eric J. Bringley (eb656)
#
# Contact:
#   Mr. Eric J. Bringley
#   Department of Chemical Engineering
#   University of Cambridge
#   West Cambridge Site
#   Philippa Fawcett Drive
#   Cambridge
#   CB3 0AS
#   United Kingdom
#
#   Email:   eb656@cam.ac.uk
#   Website: como.cheng.cam.ac.uk
#
# Purpose:
#   This script takes streamlines from OpenFOAM and formats them as inputs for
#   the TiO2 (TTIP) particle models in MOPS.
#
# Questions for Casper/Noel:
#   1. Velocities are reported in magnitudes, therefore, I assume this is
#        the magnitude of the velocity projected into the streamline.
#        Is this correct?
#   2. Time is reported in two locations: Mops.inx and gasphase.csv -- and they
#       have the same values... Does this include U and U_thermophoretic?
#       Ans: From create_mops.m in preprint 204 archive time us from convective
#           and thermophoretic veolicty.
#   3. In sweep.xml files, is Ru autodefined?
#       Ans: Ru was defined in <formula> of sweep.xml, which is not read
#       by MOPS. It is only there to act as a comment.
#   4. sweep: Ensemble contracting too often; possible stiffness issue.
#       What is this and how do you overcome it?
#
# *****************************************************************************

import sys
import os
import re
import numpy as np
import matplotlib.pyplot as plt
import pandas as pd
import xml.etree.ElementTree as ET
import xml.dom.minidom as DOM

# src = "FOAM"
src = "kinetics"


def project(vector1, vector2):
    # Functions to assist calculations
    "Project vector1 onto vector 2"
    # check lengths of vectors:
    vector2_length = np.sqrt(dot(vector2, vector2))
    vector2_unit = vector2/vector2_length
    projection = dot(vector1, vector2_unit)
    return projection


def dot(v1, v2):
    if (len(v1) != 2):
        raise ValueError
    if (len(v2) != 2):
        raise ValueError
    return v1[0]*v2[0] + v1[1]*v2[1]


# Write Input Files.
# print pretty
def prettify(elem):
    """Return a pretty-printed XML string for the Element.
    """
    rough_string = ET.tostring(elem, 'utf-8')
    reparsed = DOM.parseString(rough_string)
    return reparsed.toprettyxml(indent="    ")


def writeStreamline(data, number, comment=""):
    """
        Write mops.inx and my_gasphase.csv for a streamline
    """

    data.to_csv("my_gasphase_"+str(number)+".csv", index=False)
    print("Wrote gas phase")
    root = ET.Element('mops')
    root.set('version', '2')
    root.append(ET.Comment('Generated by writeMOPSinx.py for MOPS input file'))
    root.append(ET.Comment(comment))
    runs = ET.SubElement(root, 'runs')
    runs.text = str(2)
    atol = ET.SubElement(root, 'atol')
    atol.text = str(1.0e-17)
    rtol = ET.SubElement(root, 'rtol')
    rtol.text = str(1.0e-4)
    maxm0 = ET.SubElement(root, 'maxm0')
    maxm0.text = str(7e+24)
    pcount = ET.SubElement(root, 'pcount')
    pcount.text = str(4096)
    reactor = ET.SubElement(root, 'reactor')
    reactor.set('type', 'batch')
    reactor.set('constt', 'false')
    reactor.set('constv', 'false')
    reactor.set('id', 'TTIP_flame')
    reactor.set('units', "mol/mol")
    temperature = ET.SubElement(reactor, 'temperature')
    temperature.set('units', "K")
    temperature.text = str(dfInputs.loc[0, 'T'])
    pressure = ET.SubElement(reactor, 'pressure')
    pressure.set('units', 'bar')
    pressure.text = str(dfInputs.loc[0, 'P'])

    component1 = ET.SubElement(reactor, 'component')
    component1.set('id', "C12H28O4TI")
    component1.text = str(dfInputs.loc[0, 'C12H28O4TI'])

    component2 = ET.SubElement(reactor, 'component')
    component2.set('id', "AR")
    component2.text = str(dfInputs.loc[0, 'AR'])

    component3 = ET.SubElement(reactor, 'component')
    component3.set('id', "O2")
    component3.text = str(dfInputs.loc[0, 'O2'])

    component4 = ET.SubElement(reactor, 'component')
    component4.set('id', "C2H4")
    component4.text = str(dfInputs.loc[0, 'C2H4'])

    # Time Intervals:
    timeintervals = ET.SubElement(root, 'timeintervals')
    start = ET.SubElement(timeintervals, 'start')
    start.text = str(0)
    # get time data points:
    times = data.loc[:, 'Time'].to_numpy()
    print(len(times))
    for i in range(1, len(times)):
        time_i = ET.SubElement(timeintervals, 'time')
        time_i.text = str(times[i])
        time_i.set('steps', str(10))
        time_i.set('splits', str(10))

    # Terminal Output:
    output = ET.SubElement(root, 'output')
    statsbound = ET.SubElement(output, 'statsbound')
    statsbound.set('property', 'dcol')
    lower = ET.SubElement(statsbound, 'lower')
    lower.text = str(0)
    upper = ET.SubElement(statsbound, 'upper')
    upper.text = str(1.0e30)
    console = ET.SubElement(output, 'console')
    console.set('interval', str(1))
    console.set('msgs', "True")
    tabular = ET.SubElement(console, 'tabular')
    column1 = ET.SubElement(tabular, 'column')
    column1.set("fmt", "sci")
    column1.text = "time"
    column2 = ET.SubElement(tabular, 'column')
    column2.set("fmt", "float")
    column2.text = "#sp"
    column3 = ET.SubElement(tabular, 'column')
    column3.set("fmt", "sci")
    column3.text = "C12H28O4TI"
    column4 = ET.SubElement(tabular, 'column')
    column4.set("fmt", "sci")
    column4.text = "M0"
    column5 = ET.SubElement(tabular, 'column')
    column5.set("fmt", "sci")
    column5.text = "M1"
    column6 = ET.SubElement(tabular, 'column')
    column6.set("fmt", "sci")
    column6.text = "T"
    # used to do point of view tracking for TEM
    ptrack = ET.SubElement(output, 'ptrack')
    ptrack.set('enable', 'false')
    ptrack.set('ptcount', str(0))
    writebintree = ET.SubElement(output, 'writebintree')
    writebintree.set('enable', 'false')
    filename = ET.SubElement(output, 'filename')
    filename.text = 'Z'+str(number)
    print("Writing inx file")
    # print to terminal and write to file:
    # print(prettify(root))
    with open('test_mops_'+str(number)+'.inx', 'w') as f:
        # overwrite if anything is there
        f.write(prettify(root))

    return None


# Inputs:
if(src == "FOAM"):
    foam_case_dir = os.path.abspath("/rds/user/eb656/hpc-work/" +
                                    "Stagnation2/20200723_mym_phi1.0_coarse_ttip_simple/")
    foam_time = 0.0025
    foam_prostProcess_dir = "postProcessing/sets/streamLines/"
    # Streamline with molefraction data split into two files due
    # to restrictions on filename length
    foam_prostProcess_dir_mech_1 = "postProcessing/sets/streamLinesMoleFracs1/"
    foam_prostProcess_dir_mech_2 = "postProcessing/sets/streamLinesMoleFracs2/"

    foam_streamline_basename = "track0"
    foam_streamline_dir = os.path.join(foam_case_dir,
                                       foam_prostProcess_dir + str(foam_time))
    foam_streamline_mech_1_dir = os.path.join(foam_case_dir,
                                              foam_prostProcess_dir_mech_1 +
                                              str(foam_time))
    foam_streamline_mech_2_dir = os.path.join(foam_case_dir,
                                              foam_prostProcess_dir_mech_2
                                              + str(foam_time))
    foam_streamline_file_scalar = os.path.join(foam_streamline_dir,
                                               foam_streamline_basename
                                               + "_p_T.csv")
    foam_streamline_file_mech_1 = os.path.join(foam_streamline_mech_1_dir,
                                               foam_streamline_basename
                                               + "_X_N2_X_O_X_CH2CO_X_C2H5_X_CH3_X_O2_X_CH2OH_X_C2H4O_X_CH3O_X_H2O2_X_T-CH2_X_C3H8_X_CH2O_X_C3H5_X_AR_X_CO_X_CH2CHO_X_C2H_X_C2H4_X_H2O_X_CO2_X_HCO.csv")
    foam_streamline_file_mech_2 = os.path.join(foam_streamline_mech_2_dir,
                                               foam_streamline_basename
                                               + "_X_H_X_C2H6_X_N-C3H7_X_HO2_X_HCCO_X_C3H3_X_C3H6_X_OH_X_C2H2_X_CH3OH_X_C3H4_X_I-C3H7_X_C2H3_X_HE_X_H2_X_CH_X_CH4_X_C12H28O4TI.csv")
    foam_streamline_file_vector = os.path.join(foam_streamline_dir,
                                               foam_streamline_basename
                                               + "_U_V_T.csv")

    # Read Streamine Velocity Data
    velocity_data = pd.read_csv(foam_streamline_file_vector)
    velocity_data = velocity_data.drop(columns=['x'])
    scalar_data = pd.read_csv(foam_streamline_file_scalar)
    scalar_data = scalar_data.drop(columns=['x'])

    # separate streamlines:
    # Assumptions:
    #   1. Streamlines start at their max height and travel downward
    #   2. Streamlines do not drift to above the starting height

    # Check raw streamlines as XY plot:
    checkRawStreamlines = False
    if(checkRawStreamlines):
        velocity_data.plot(x='y', y='z', kind='line')
        plt.show()

    # find height of starting streamline
    start_z = velocity_data['z'].max()
    print("Starting height {}".format(start_z))
    streamline_idx = np.empty_like(velocity_data['z'])

    # Calculate streamline indexes:
    first_steamline_index = 0
    idx = first_steamline_index-1

    # streamline info:
    t_res = 0
    u_mag = 0

    streamline_y = velocity_data['y']
    streamline_z = velocity_data['z']
    streamline_uy = velocity_data['U_1']  # velocity y
    streamline_uz = velocity_data['U_2']  # velocity z
    streamline_vy = velocity_data['V_T_1']  # thermophoretic velocity y
    streamline_vz = velocity_data['V_T_2']  # thermophoretic velocity z

    for i in range(len(streamline_z)):
        # Strreamline idx:
        height = streamline_z[i]
        if (height == start_z):
            print("New streamline")
            idx = idx+1
        streamline_idx[i] = idx
        # print("Height is {} in steamline {}".format(height,idx))

    # Store streamline index
    velocity_data['streamline'] = streamline_idx
    velocity_data['streamline'] = velocity_data['streamline'].astype('int')
    # convert to integer!
    scalar_data['streamline'] = streamline_idx
    scalar_data['streamline'] = scalar_data['streamline'].astype('int')

    # Calculate residence time and velocity magnitudes:
    umag_full = np.zeros(0)
    vmag_full = np.zeros(0)
    tres_full = np.zeros(0)

    # direction is calculated as change from before and after points.
    # distance is change from previous point to current point

    for j in range(0, velocity_data['streamline'].astype('int').max()+1):
        print("Post Processing Streamline {}".format(j))
        dfStreamline = velocity_data[velocity_data['streamline'] == j]
        streamline_y = dfStreamline['y'].to_numpy()
        streamline_z = dfStreamline['z'].to_numpy()
        streamline_uy = dfStreamline['U_1'].to_numpy()  # velocity y
        streamline_uz = dfStreamline['U_2'].to_numpy()  # velocity z
        streamline_vy = dfStreamline['V_T_1'].to_numpy()  # thermophoretic v_y
        streamline_vz = dfStreamline['V_T_2'].to_numpy()  # thermophoretic v_z
        streamline_umag = np.empty_like(dfStreamline['z'])
        streamline_vmag = np.empty_like(dfStreamline['z'])
        streamline_tres = np.empty_like(dfStreamline['z'])
        # intial point:
        position_i = [streamline_y[0], streamline_z[0]]
        u = [streamline_uy[0], streamline_uz[0]]
        v = [streamline_vy[0], streamline_vz[0]]
        position_next = [streamline_y[1], streamline_z[1]]
        on_to_direction = [position_next[0]-position_i[0],
                           position_next[1]-position_i[1]]
        streamline_umag[0] = project(u, on_to_direction)
        streamline_vmag[0] = project(v, on_to_direction)
        streamline_tres[0] = 0
        # internal points:
        for i in range(1, len(streamline_z)-1):
            position_i = [streamline_y[i], streamline_z[i]]
            u = [streamline_uy[i], streamline_uz[i]]
            v = [streamline_vy[i], streamline_vz[i]]
            position_prev = [streamline_y[i-1], streamline_z[i-1]]
            position_next = [streamline_y[i+1], streamline_z[i+1]]
            on_to_direction = [position_next[0]-position_prev[0],
                               position_next[1]-position_prev[1]]
            u_v = [u[0]+v[0], u[1]+v[1]]
            u_total = project(u_v, u_v)
            dx = [position_i[0]-position_prev[0],
                  position_i[1]-position_prev[1]]
            distance = project(dx, dx)
            tres_i = distance/u_total
            streamline_umag[i] = project(u, on_to_direction)
            streamline_vmag[i] = project(v, on_to_direction)
            streamline_tres[i] = tres_i+streamline_tres[i-1]
        # final point:
        position_i = [streamline_y[-1], streamline_z[-1]]
        position_prev = [streamline_y[-2], streamline_z[-2]]
        u = [streamline_uy[-1], streamline_uz[-1]]
        v = [streamline_vy[-1], streamline_vz[-1]]
        on_to_direction = [position_i[0]-position_prev[0],
                           position_i[1] - position_prev[1]]
        u_v = [u[0]+v[0], u[1]+v[1]]
        u_total = project(u_v, u_v)
        dx = [position_i[0]-position_prev[0],
              position_i[1]-position_prev[1]]
        distance = project(dx, dx)
        tres_i = distance/u_total
        streamline_umag[-1] = project(u, on_to_direction)
        streamline_vmag[-1] = project(v, on_to_direction)
        streamline_tres[-1] = streamline_tres[-2] + tres_i
        # print(streamline_umag)
        # append data to full lists:
        umag_full = np.concatenate([umag_full, streamline_umag])
        vmag_full = np.concatenate([vmag_full, + streamline_vmag])
        tres_full = np.concatenate([tres_full, + streamline_tres])

    print(len(umag_full))
    print(len(streamline_idx))
    velocity_data['ConvectiveVelocity[m/s]'] = umag_full
    velocity_data['ThermophoreticVelocity[m/s]'] = vmag_full
    velocity_data['Time'] = tres_full

    print(velocity_data)
    print(velocity_data.dtypes)

    # Create master dataframe:
    dfInputs = pd.DataFrame()
    dfInputs['streamline'] = scalar_data['streamline'].astype('int')
    dfInputs['ConvectiveVelocity[m/s]'] = umag_full
    dfInputs['ThermophoreticVelocity[m/s]'] = vmag_full
    dfInputs['Time'] = tres_full
    dfInputs['T'] = scalar_data['T']
    dfInputs['P'] = scalar_data['p'] / 100000  # pa to bar
    dfInputs['wdotA4'] = scalar_data['p']*0
    dfInputs['DiffusionTerm'] = scalar_data['p']*0

    # Read Mole Fraction (X) data
    mole_fraction_1 = pd.read_csv(foam_streamline_file_mech_1)
    mole_fraction_1 = mole_fraction_1.drop(columns=['x', 'y', 'z'])
    mole_fraction_2 = pd.read_csv(foam_streamline_file_mech_2)
    mole_fraction_2 = mole_fraction_2.drop(columns=['x', 'y', 'z'])
    mole_fraction_1['streamline'] = scalar_data['streamline'].astype('int')
    mole_fraction_2['streamline'] = scalar_data['streamline'].astype('int')

    print(mole_fraction_2)
    # Add mole fraction to dfInputs)
    columns_1 = mole_fraction_1.columns
    columns_2 = mole_fraction_2.columns
    # remove mole fraction name
    pattern = re.compile("X_")
    for item in columns_1:
        specieName = re.sub(pattern, "", item)
        dfInputs[specieName] = mole_fraction_1[item]
    for item in columns_2:
        specieName = re.sub(pattern, "", item)
        dfInputs[specieName] = mole_fraction_2[item]
    # print(mole_fraction_1.dtypes)
    # print(mole_fraction_2.dtypes)
    # print(dfInputs.dtypes)
    for j in range(0, velocity_data['streamline'].astype('int').max()+1):
        data = dfInputs[dfInputs['streamline'] == j]
        print("Writing streamline {}".format(j))
        writeStreamline(data, j, comment="case: {}".format(foam_case_dir))

elif(src == "kinetics"):
    # Python Module to read kinetics case data:
    import kinetics as kin
    hpcwork = os.path.abspath("/home/eb656/hpc-work/")
    wdir = "Stagnation2/20200619_mym_phi1.0/StagnationFlameWithTTIP"
    case = kin.case(os.path.join(hpcwork, wdir))
    case.readOutputFiles()
    grid = case['FlameThermofluids']['Length [m]']
    temperature = case['FlameThermofluids']['Temperature [K]']
    pressure = case['FlameThermofluids']['Pressure [bar]']*101325.0
    # Extract centerline velocity data:
    u = case['FlameNano']['Convective velocity [cm/s]']/100.0
    v = case['FlameNano']['Thermophoretic velocity [cm/s]']/100.0
    time = np.empty_like(grid)
    # print("{} {} {}".format(max(temperature),max(u),max(v)))
    time[0] = 0
    for i in range(1, len(time)):
        dx = grid[i]-grid[i-1]
        velocity = (u[i]+v[i]+u[i-1]+v[i-1])/2.0  # average of point i & i-1
        dt = dx/velocity
        time[i] = time[i-1]+dt
    print(time)
    # make input data frame:
    dfInputs = pd.DataFrame()
    dfInputs['ConvectiveVelocity[m/s]'] = u
    dfInputs['ThermophoreticVelocity[m/s]'] = v
    dfInputs['Time'] = time
    dfInputs['T'] = temperature
    dfInputs['P'] = pressure / 100000.0  # pa to bar
    dfInputs['wdotA4'] = pressure*0
    dfInputs['DiffusionTerm'] = pressure*0

    # Collect Mole Fractions:
    moleFracs = case['MoleFracs']
    columns = moleFracs.columns
    mycolumns = [x for x in columns if x != "Length [cm]" and x != "Length [m]"]
    # remove mole fraction name
    pattern = re.compile(" mole fraction \[\-\]")
    for item in mycolumns:
        specieName = re.sub(pattern, "", item)
        dfInputs[specieName] = moleFracs[item]
    print(dfInputs.columns)

    # Write Streamline:
    writeStreamline(dfInputs, -1, comment="case: {}".format(case.path()))
else:
    print("please specify kinetics or FOAM for your data source.")
