Import("env")

''' Fixes getting unit test results from trinket '''
def after_upload(source, target, env):
    print ("Uploading in 1 sec...")
    import time
    time.sleep(1)
    print ("Ready!")

env.AddPostAction("upload", after_upload)