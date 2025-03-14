# trace generated using paraview version 5.10.1
import paraview
paraview.compatibility.major = 5
paraview.compatibility.minor = 10



#### USER EDITS ####

# Specify the full path of the .dat file
instructionFilePath = "../output_dir/instructions.dat"

# Initialize an empty list to store the string variables
string_variables = []

# Open the .dat file in read mode
with open(instructionFilePath, 'r') as file:
    # Read lines from the file
    lines = file.readlines()

    inputDirectory = lines[0].strip()
    inputFileName = lines[2].strip()
    inputNacaDirPlusName = lines[0].strip() + lines[5].strip()

    outputFileName1 = lines[6].strip() + ".png"
    outputFileName2 = lines[7].strip() + ".png"
    outputDirectory = lines[1].strip()

    machUpper = float(lines[10].strip())
    machLower = float(lines[11].strip())
    cpUpper = float(lines[12].strip())
    cpLower = float(lines[13].strip())

print(inputNacaDirPlusName)
print(outputFileName1)

length = 30
stepMach = 1.0 / length * (machUpper - machLower)
stepCP = 1.0 / length * (cpUpper - cpLower)

machVector = []
cpVector = []

for i in range(length + 1):
    machVector.append(machLower + i * stepMach)
    cpVector.append(cpLower + i * stepCP)

# image size
size = [1100, 700]

#### END OF USER EDITS ####



#### import the simple module from the paraview
from paraview.simple import *
#### disable automatic camera reset on 'Show'
paraview.simple._DisableFirstRenderCameraReset()

# create a new 'CSV Reader'
##############################################################################################################################################################################
meshVertices = CSVReader(registrationName='naca_mesh.csv', FileName=inputDirectory + inputFileName)
##############################################################################################################################################################################

# Create a new 'SpreadSheet View'
spreadSheetView1 = CreateView('SpreadSheetView')
spreadSheetView1.ColumnToSort = ''
spreadSheetView1.BlockSize = 1024

# show data in view
meshVerticesDisplay = Show(meshVertices, spreadSheetView1, 'SpreadSheetRepresentation')

# get layout
layout1 = CreateLayout(name="Layout #1")

# layout/tab size in pixels
layout1.SetSize(size)

# add view to a layout so it's visible in UI
AssignViewToLayout(view=spreadSheetView1, layout=layout1, hint=0)

# Properties modified on meshVerticesDisplay
meshVerticesDisplay.Assembly = ''

# find view
renderView1 = FindViewOrCreate('RenderView1', viewtype='RenderView')
renderView1.ViewSize = size

# update the view to ensure updated data information
renderView1.Update()

# update the view to ensure updated data information
spreadSheetView1.Update()

# create a new 'Table To Points'
tableToPoints1 = TableToPoints(registrationName='TableToPoints1', Input=meshVertices)
tableToPoints1.XColumn = 'X'
tableToPoints1.YColumn = 'X'
tableToPoints1.ZColumn = 'X'

# Properties modified on tableToPoints1
tableToPoints1.YColumn = 'Y'
tableToPoints1.ZColumn = 'Z'
tableToPoints1.a2DPoints = 1

# show data in view
tableToPoints1Display = Show(tableToPoints1, spreadSheetView1, 'SpreadSheetRepresentation')

# hide data in view
Hide(meshVertices, spreadSheetView1)

# update the view to ensure updated data information
spreadSheetView1.Update()

# Properties modified on tableToPoints1Display
tableToPoints1Display.Assembly = ''

# create a new 'Delaunay 2D'
delaunay2D1 = Delaunay2D(registrationName='Delaunay2D1', Input=tableToPoints1)

# show data in view
delaunay2D1Display = Show(delaunay2D1, spreadSheetView1, 'SpreadSheetRepresentation')

# hide data in view
Hide(tableToPoints1, spreadSheetView1)

# update the view to ensure updated data information
spreadSheetView1.Update()

# Properties modified on delaunay2D1Display
delaunay2D1Display.Assembly = ''

# create a new 'Contour'
contour1 = Contour(registrationName='Contour1', Input=delaunay2D1)
contour1.ContourBy = ['POINTS', 'M']
contour1.Isosurfaces = [0.70896]
contour1.PointMergeMethod = 'Uniform Binning'

# Properties modified on contour1
##############################################################################################################################################################################
contour1.Isosurfaces = machVector
#contour1.Isosurfaces = [0.10971, 0.15103758620689656, 0.1923651724137931, 0.23369275862068967, 0.2750203448275862, 0.3163479310344828, 0.35767551724137936, 0.39900310344827583, 0.4403306896551724, 0.481658275862069, 0.5229858620689656, 0.564313448275862, 0.6056410344827586, 0.6469686206896552, 0.6882962068965517, 0.7296237931034483, 0.7709513793103449, 0.8122789655172414, 0.853606551724138, 0.8949341379310345, 0.9362617241379311, 0.9775893103448277, 1.0189168965517241, 1.0602444827586208, 1.1015720689655173, 1.1428996551724138, 1.1842272413793105, 1.225554827586207, 1.2668824137931034, 1.30821]
##############################################################################################################################################################################

# show data in view
contour1Display = Show(contour1, spreadSheetView1, 'SpreadSheetRepresentation')

# update the view to ensure updated data information
spreadSheetView1.Update()

# Properties modified on contour1Display
contour1Display.Assembly = ''

# create a new 'CSV Reader'
##############################################################################################################################################################################
onlynacacsv = CSVReader(registrationName='only-naca.csv', FileName=[inputNacaDirPlusName])
##############################################################################################################################################################################

# show data in view
onlynacacsvDisplay = Show(onlynacacsv, spreadSheetView1, 'SpreadSheetRepresentation')

# update the view to ensure updated data information
spreadSheetView1.Update()

# Properties modified on onlynacacsvDisplay
onlynacacsvDisplay.Assembly = ''

# create a new 'Table To Points'
tableToPoints2 = TableToPoints(registrationName='TableToPoints2', Input=onlynacacsv)
tableToPoints2.XColumn = 'X'
tableToPoints2.YColumn = 'X'
tableToPoints2.ZColumn = 'X'

# Properties modified on tableToPoints2
tableToPoints2.YColumn = 'Y'
tableToPoints2.ZColumn = 'Z'

# show data in view
tableToPoints2Display = Show(tableToPoints2, spreadSheetView1, 'SpreadSheetRepresentation')

# hide data in view
Hide(onlynacacsv, spreadSheetView1)

# update the view to ensure updated data information
spreadSheetView1.Update()

# Properties modified on tableToPoints2Display
tableToPoints2Display.Assembly = ''

# create a new 'Delaunay 2D'
delaunay2D2 = Delaunay2D(registrationName='Delaunay2D2', Input=tableToPoints2)

# show data in view
delaunay2D2Display = Show(delaunay2D2, spreadSheetView1, 'SpreadSheetRepresentation')

# hide data in view
Hide(tableToPoints2, spreadSheetView1)

# update the view to ensure updated data information
spreadSheetView1.Update()

# Properties modified on delaunay2D2Display
delaunay2D2Display.Assembly = ''

# set active view
SetActiveView(renderView1)

# set active view
SetActiveView(spreadSheetView1)

# destroy spreadSheetView1
Delete(spreadSheetView1)
del spreadSheetView1

# set active view
SetActiveView(renderView1)

#change interaction mode for render view
renderView1.InteractionMode = '2D'

# get the material library
materialLibrary1 = GetMaterialLibrary()

# set active source
SetActiveSource(delaunay2D1)

# show data in view
delaunay2D1Display = Show(delaunay2D1, renderView1, 'GeometryRepresentation')

# trace defaults for the display properties.
delaunay2D1Display.Representation = 'Surface'
delaunay2D1Display.ColorArrayName = [None, '']
delaunay2D1Display.SelectTCoordArray = 'None'
delaunay2D1Display.SelectNormalArray = 'None'
delaunay2D1Display.SelectTangentArray = 'None'
delaunay2D1Display.OSPRayScaleArray = 'M'
delaunay2D1Display.OSPRayScaleFunction = 'PiecewiseFunction'
delaunay2D1Display.SelectOrientationVectors = 'None'
delaunay2D1Display.ScaleFactor = 3.9977400000000003
delaunay2D1Display.SelectScaleArray = 'None'
delaunay2D1Display.GlyphType = 'Arrow'
delaunay2D1Display.GlyphTableIndexArray = 'None'
delaunay2D1Display.GaussianRadius = 0.199887
delaunay2D1Display.SetScaleArray = ['POINTS', 'M']
delaunay2D1Display.ScaleTransferFunction = 'PiecewiseFunction'
delaunay2D1Display.OpacityArray = ['POINTS', 'M']
delaunay2D1Display.OpacityTransferFunction = 'PiecewiseFunction'
delaunay2D1Display.DataAxesGrid = 'GridAxesRepresentation'
delaunay2D1Display.PolarAxes = 'PolarAxesRepresentation'

# init the 'PiecewiseFunction' selected for 'OSPRayScaleFunction'
delaunay2D1Display.OSPRayScaleFunction.Points = [0.12271874482758621, 0.0, 0.5, 0.0, 0.8465339999999999, 1.0, 0.5, 0.0]

# init the 'PiecewiseFunction' selected for 'ScaleTransferFunction'
delaunay2D1Display.ScaleTransferFunction.Points = [0.10971, 0.0, 0.5, 0.0, 1.30821, 1.0, 0.5, 0.0]

# init the 'PiecewiseFunction' selected for 'OpacityTransferFunction'
delaunay2D1Display.OpacityTransferFunction.Points = [0.10971, 0.0, 0.5, 0.0, 1.30821, 1.0, 0.5, 0.0]

#changing interaction mode based on data extents
renderView1.CameraPosition = [-7.286955000000001, 0.0, 10000.0]
renderView1.CameraFocalPoint = [-7.286955000000001, 0.0, 0.0]

# reset view to fit data
renderView1.ResetCamera(False)

# set scalar coloring
ColorBy(delaunay2D1Display, ('POINTS', 'M'))

# rescale color and/or opacity maps used to include current data range
delaunay2D1Display.RescaleTransferFunctionToDataRange(True, False)

# show color bar/color legend
# delaunay2D1Display.SetScalarBarVisibility(renderView1, True)

# get color transfer function/color map for 'M'
MLUT = GetColorTransferFunction('M')
MLUT.RGBPoints = [0.10971, 0.231373, 0.298039, 0.752941, 0.70896, 0.865003, 0.865003, 0.865003, 1.30821, 0.705882, 0.0156863, 0.14902]
MLUT.ScalarRangeInitialized = 1.0

# get opacity transfer function/opacity map for 'M'
MPWF = GetOpacityTransferFunction('M')
MPWF.Points = [0.10971, 0.0, 0.5, 0.0, 1.30821, 1.0, 0.5, 0.0]
MPWF.ScalarRangeInitialized = 1

# set active source
SetActiveSource(contour1)

# show data in view
contour1Display = Show(contour1, renderView1, 'GeometryRepresentation')

# trace defaults for the display properties.
contour1Display.Representation = 'Surface'
contour1Display.ColorArrayName = ['POINTS', 'M']
contour1Display.LookupTable = MLUT
contour1Display.SelectTCoordArray = 'None'
contour1Display.SelectNormalArray = 'None'
contour1Display.SelectTangentArray = 'None'
contour1Display.OSPRayScaleArray = 'M'
contour1Display.OSPRayScaleFunction = 'PiecewiseFunction'
contour1Display.SelectOrientationVectors = 'None'
contour1Display.ScaleFactor = 0.5652918505023614
contour1Display.SelectScaleArray = 'M'
contour1Display.GlyphType = 'Arrow'
contour1Display.GlyphTableIndexArray = 'M'
contour1Display.GaussianRadius = 0.02826459252511807
contour1Display.SetScaleArray = ['POINTS', 'M']
contour1Display.ScaleTransferFunction = 'PiecewiseFunction'
contour1Display.OpacityArray = ['POINTS', 'M']
contour1Display.OpacityTransferFunction = 'PiecewiseFunction'
contour1Display.DataAxesGrid = 'GridAxesRepresentation'
contour1Display.PolarAxes = 'PolarAxesRepresentation'

# init the 'PiecewiseFunction' selected for 'OSPRayScaleFunction'
contour1Display.OSPRayScaleFunction.Points = [0.12271874482758621, 0.0, 0.5, 0.0, 0.8465339999999999, 1.0, 0.5, 0.0]

# init the 'PiecewiseFunction' selected for 'ScaleTransferFunction'
contour1Display.ScaleTransferFunction.Points = [0.15103758620689656, 0.0, 0.5, 0.0, 1.3082099999999999, 1.0, 0.5, 0.0]

# init the 'PiecewiseFunction' selected for 'OpacityTransferFunction'
contour1Display.OpacityTransferFunction.Points = [0.15103758620689656, 0.0, 0.5, 0.0, 1.3082099999999999, 1.0, 0.5, 0.0]

# show color bar/color legend
contour1Display.SetScalarBarVisibility(renderView1, True)

# set active source
SetActiveSource(delaunay2D2)

# show data in view
delaunay2D2Display = Show(delaunay2D2, renderView1, 'GeometryRepresentation')

# trace defaults for the display properties.
delaunay2D2Display.Representation = 'Surface'
delaunay2D2Display.ColorArrayName = [None, '']
delaunay2D2Display.SelectTCoordArray = 'None'
delaunay2D2Display.SelectNormalArray = 'None'
delaunay2D2Display.SelectTangentArray = 'None'
delaunay2D2Display.OSPRayScaleArray = 'M'
delaunay2D2Display.OSPRayScaleFunction = 'PiecewiseFunction'
delaunay2D2Display.SelectOrientationVectors = 'None'
delaunay2D2Display.ScaleFactor = 0.1
delaunay2D2Display.SelectScaleArray = 'None'
delaunay2D2Display.GlyphType = 'Arrow'
delaunay2D2Display.GlyphTableIndexArray = 'None'
delaunay2D2Display.GaussianRadius = 0.005
delaunay2D2Display.SetScaleArray = ['POINTS', 'M']
delaunay2D2Display.ScaleTransferFunction = 'PiecewiseFunction'
delaunay2D2Display.OpacityArray = ['POINTS', 'M']
delaunay2D2Display.OpacityTransferFunction = 'PiecewiseFunction'
delaunay2D2Display.DataAxesGrid = 'GridAxesRepresentation'
delaunay2D2Display.PolarAxes = 'PolarAxesRepresentation'

# init the 'PiecewiseFunction' selected for 'OSPRayScaleFunction'
delaunay2D2Display.OSPRayScaleFunction.Points = [0.12271874482758621, 0.0, 0.5, 0.0, 0.8465339999999999, 1.0, 0.5, 0.0]

# init the 'PiecewiseFunction' selected for 'ScaleTransferFunction'
delaunay2D2Display.ScaleTransferFunction.Points = [0.634335, 0.0, 0.5, 0.0, 0.6344570517539978, 1.0, 0.5, 0.0]

# init the 'PiecewiseFunction' selected for 'OpacityTransferFunction'
delaunay2D2Display.OpacityTransferFunction.Points = [0.634335, 0.0, 0.5, 0.0, 0.6344570517539978, 1.0, 0.5, 0.0]

# set active source
SetActiveSource(delaunay2D1)

# Apply a preset using its name. Note this may not work as expected when presets have duplicate names.
MLUT.ApplyPreset('Viridis (matplotlib)', True)

# rescale color and/or opacity maps used to exactly fit the current data range
delaunay2D1Display.RescaleTransferFunctionToDataRange(False, True)

# Hide orientation axes
renderView1.OrientationAxesVisibility = 0

# current camera placement for renderView1
renderView1.InteractionMode = '2D'
renderView1.CameraPosition = [0.5567216570651826, 0.029935311733647878, 90]
renderView1.CameraFocalPoint = [0.5567216570651826, 0.029935311733647878, 0.0]
renderView1.CameraParallelScale = 1.9363696273666482

##############################################################################################################################################################################
# get color transfer function/color map for 'M'
mLUT = GetColorTransferFunction('M')

# get opacity transfer function/opacity map for 'M'
mPWF = GetOpacityTransferFunction('M')

# Apply a preset using its name. Note this may not work as expected when presets have duplicate names.
mLUT.ApplyPreset('Viridis (matplotlib)', True)

# get color legend/bar for mLUT in view renderView1
mLUTColorBar = GetScalarBar(mLUT, renderView1)
mLUTColorBar.Title = 'M'
mLUTColorBar.ComponentTitle = ''

# change scalar bar placement
mLUTColorBar.Position = [0.8, 0.031725888324873094]
mLUTColorBar.ScalarBarLength = 0.33
##############################################################################################################################################################################

# save screenshot
##############################################################################################################################################################################
SaveScreenshot(outputDirectory + outputFileName1, renderView1, ImageResolution=[1920, 1080],
##############################################################################################################################################################################
    TransparentBackground=1, 
    # PNG options
    CompressionLevel='0')

# current camera placement for renderView1
renderView1.InteractionMode = '2D'
renderView1.CameraPosition = [0.43921224764053707, 0.008356424981627433, 90]
renderView1.CameraFocalPoint = [0.43921224764053707, 0.008356424981627433, 0.0]
renderView1.CameraParallelScale = 0.6169870377573335

# save screenshot
##############################################################################################################################################################################
SaveScreenshot(outputDirectory + outputFileName2, renderView1, ImageResolution=[1920, 1080],
##############################################################################################################################################################################
    TransparentBackground=1, 
    # PNG options
    CompressionLevel='0')